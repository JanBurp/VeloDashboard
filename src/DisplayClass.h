#pragma once

#include "Arduino.h"
#include "settings.h"
#include "SpeedClass.h"
#include "BatteryClass.h"
#include "IdleClass.h"
#include "IndicatorClass.h"
#include "LightsClass.h"
#include "LEDstripClass.h"

#include "Icons.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 OLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

class DisplayClass
{

private:
    int displayMode = DISPLAY_HOME;
    int firstMode = DISPLAY_HOME;
    int lastMode = DISPLAY_TOTALS;
    int firstSettingsMode = DISPLAY_SETTINGS_TRIP;
    int lastSettingsMode = DISPLAY_SETTINGS_TOTAL;
    bool settingsMenu = false;
    int cursorPosition = 0;

    SpeedClass *Speed;
    BatteryClass *Battery;
    IdleClass *IdleTimer;
    IndicatorClass *Indicators;
    LightsClass *Lights;
    LEDstripClass *LEDstrips;

public:
    void init(SpeedClass *speed, BatteryClass *battery, IdleClass *idle, IndicatorClass *indicators, LightsClass *lights, LEDstripClass *LEDstrips)
    {
        this->Speed = speed;
        this->Battery = battery;
        this->IdleTimer = idle;
        this->Indicators = indicators;
        this->Lights = lights;
        this->LEDstrips = LEDstrips;
        // Address 0x3D for 128x64
        if (!OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C))
        {
            Serial.println(F("SSD1306 allocation failed"));
            for (;;)
            {
                ;
            }
        }
        this->setContrast(128);
        OLED.clearDisplay();
        OLED.setTextColor(WHITE);
    }

    void setDisplayModeHome() {
        this->setDisplayMode(DISPLAY_HOME);
    }

    void setDisplayMode(int mode)
    {
        this->displayMode = mode;
    }

    void nextDisplayMode()
    {
        this->resetCursor();
        this->displayMode++;
        if ( this->settingsMenu ) {
            if ( this->displayMode > this->lastSettingsMode )
            {
                this->displayMode = this->firstSettingsMode;
            }
        }
        else {
            if (this->displayMode > this->lastMode)
            {
                this->displayMode = this->firstMode;
            }
        }
    }

    void resetDisplayMode()
    {
        this->displayMode = DISPLAY_HOME;
    }

    void show_welcome()
    {
        OLED.setTextSize(2);
        OLED.setCursor(0, 0);
        OLED.print("Quest 631");
    }

    void show_speed()
    {
        OLED.setTextColor(WHITE);
        if ( this->Speed->isStarted() ) {

            float speed = this->Speed->getSpeed();
            int decis = (int)speed;
            int precision = (speed - decis) * 10;

            int x = SCREEN_HALF_WIDTH + 4;
            if ( this->displayMode == DISPLAY_HOME ) {
                x = 38;
            }

            OLED.setTextSize(5);
            char speedStr[3];
            snprintf(speedStr, 3, "%2i", decis);
            OLED.setCursor(x, 0);
            OLED.print(speedStr);

            if ( this->displayMode == DISPLAY_HOME ) {
                snprintf(speedStr, 3, "%1i", precision);
                OLED.setTextSize(3);
                OLED.setCursor(110, 14);
                OLED.print(speedStr);
                // Sensor
                OLED.setTextSize(2);
                OLED.setCursor( x+58, 22 );
                if (this->Speed->getSpeedSensor())
                {
                    OLED.print(".");
                }
                else
                {
                    OLED.print(" ");
                }
            }


            // Faster / Slower than average
            if ( !this->Speed->isPaused() && this->displayMode == DISPLAY_HOME) {
                int x = 110;
                int y = 0;
                int w = 14;
                int h = 10;
                if (this->Speed->isFaster())
                {
                    OLED.fillTriangle(x, y + h, x + w / 2, y, x + w, y + h, WHITE);
                }
                else
                {
                    OLED.fillTriangle(x, y, x + w / 2, y + h, x + w, y, WHITE);
                }
            }

        }

    }

    void show_indicators()
    {
        if (this->Indicators->getStateLeft())
        {
            OLED.fillTriangle(0, SCREEN_HALF_HEIGHT, SCREEN_HALF_WIDTH - 2, 0, SCREEN_HALF_WIDTH - 2, SCREEN_HEIGHT, WHITE);
        }
        if (this->Indicators->getStateRight())
        {
            OLED.fillTriangle(SCREEN_HALF_WIDTH + 2, 0, SCREEN_WIDTH, SCREEN_HALF_HEIGHT, SCREEN_HALF_WIDTH + 2, SCREEN_HEIGHT, WHITE);
        }
    }

    void show_time()
    {
        OLED.setTextColor(WHITE);
        char hourStr[3];
        char minStr[3];
        if ( this->IdleTimer->warning() ) {
            // Idle clock
            snprintf(minStr, 3, "%02i", this->IdleTimer->remainingSeconds() );
        }
        else {
            // Normal clock
            snprintf(hourStr, 3, "%2i", hour());
            snprintf(minStr, 3, "%02i", minute());
        }


        OLED.setTextSize(3);
        if ( !this->IdleTimer->warning() ) {
            OLED.setCursor(49, SCREEN_HALF_HEIGHT_VALUES - 6);
            OLED.print(hourStr);
        }
        OLED.setCursor(92, SCREEN_HALF_HEIGHT_VALUES - 6);
        OLED.print(minStr);

        if ( !this->IdleTimer->warning() ) {
            OLED.setTextSize(2);
            OLED.setCursor(82, SCREEN_HALF_HEIGHT_VALUES + 3);
            if ((millis() / 500) % 2 == 0)
            {
                OLED.print(":");
            }
            else
            {
                OLED.print(" ");
            }
        }
    }

    void show_battery( bool off = false)
    {
        OLED.setTextColor(WHITE);
        int toppad = 4;
        int innerpad = 2;
        int x = 1;
        int y = SCREEN_HALF_HEIGHT_INFO - 2;
        int w = 14;
        int h = 22;
        OLED.drawRect(x + toppad, y, x + w - 2 * toppad, toppad, WHITE);
        OLED.drawRect(x, y + toppad, x + w, h, WHITE);

        if ( !off ) {
            int percentage = this->Battery->getBatteryPercentage();
            int juiceHeight = (h - innerpad * 2) * (percentage/100.0);
            OLED.fillRect(x + innerpad, y + innerpad + (h - juiceHeight), x + w - 2 * innerpad, juiceHeight, WHITE);

            // print cell voltage
            int cellMilliV = this->Battery->getCellVoltage();
            int cellV = cellMilliV / 1000;
            int cellM = (cellMilliV - (1000*cellV)) / 10;
            char cellMstr[4];
            snprintf(cellMstr, 3, "%02i", cellM);

            OLED.setTextSize(2);
            OLED.setCursor(x + w + 5, y + toppad);
            OLED.print( cellV );
            OLED.setTextSize(1);
            OLED.setCursor(x + w + 17, y + toppad);
            OLED.print( cellMstr );

            // print percentage
            OLED.setCursor(x + w + 5, y + toppad + 15);
            OLED.setTextSize(1);
            char percStr[4];
            snprintf(percStr, 4, "%02i%%", percentage);
            OLED.print(percStr);
        }
        else {
            OLED.setCursor(x + w + 12, y + toppad);
            OLED.setTextSize(3);
            OLED.print(this->Battery->secondsUntilPowerOff());
        }
    }

    void show_lights()
    {
        // headlights
        int x = 0;
        int y = 0;
        int yr = 20;
        int w = 24;
        int h = 14;
        switch (this->Lights->getLights())
        {
        case LIGHTS_OFF:
            OLED.drawBitmap(x, y, icoLowBeam, w, h, WHITE);
            break;
        case LIGHTS_DIM:
            OLED.drawBitmap(x, y, icoDefaultBeam, w, h, WHITE);
            OLED.drawBitmap(x, yr, icoLowRear, w, h, WHITE);
            break;
        case LIGHTS_NORMAL:
            OLED.drawBitmap(x, y, icoHighBeam, w, h, WHITE);
            OLED.drawBitmap(x, yr, icoLowRear, w, h, WHITE);
            break;
        case LIGHTS_BEAM:
            OLED.drawBitmap(x, y, icoMaxBeam, w, h, WHITE);
            OLED.drawBitmap(x, yr, icoLowRear, w, h, WHITE);
            break;
        case LIGHTS_FOG:
            OLED.drawBitmap(x, y, icoMaxBeam, w, h, WHITE);
            OLED.drawBitmap(x, yr, icoHighRear, w, h, WHITE);
            break;
        }
        if (this->Lights->getBrake())
        {
            OLED.drawBitmap(x, yr, icoBrakeRear, w, h, WHITE);
        }

        if (this->Lights->getHorn()) {
            OLED.setCursor(x + w, y);
            OLED.setTextSize(2);
            OLED.print("!");
        }
    }

    void show_today_distances()
    {
        std::string format = "% 6.2f";
        if ( this->Speed->getDayDistance() >= 100 ) {
            format = "% 6.1f";
        }
        this->show_item_float(1,"DIST",format,this->Speed->getDistance() );
        this->show_item_float(2,"TODAY",format,this->Speed->getDayDistance() );
    }

    void show_today_speeds()
    {
        this->show_item_float(1,"AVG km/u","% 6.1f", this->Speed->getAvgSpeed());
        this->show_item_float(2,"MAX km/u","% 6.1f", this->Speed->getMaxSpeed());
    }

    void show_today_time()
    {
        this->show_item_time(1,"DRIVE", this->Speed->getTripTime());
        this->show_item_time(2,"TOTAL", this->Speed->getTotalTime());
    }

    void show_prev_dist() {
        std::string format = "% 6.2f";
        if ( this->Speed->getTripDistance() >= 100 ) {
            format = "% 6.1f";
        }
        this->show_item_float(1,"PREV",format,this->Speed->getPrevDistance() );
        this->show_item_float(2,"TRIP",format,this->Speed->getTripDistance() );
    }

    void show_prev_speed() {
        this->show_item_float(1,"AVG km/u","% 6.1f", this->Speed->getPrevAvgSpeed());
        this->show_item_float(2,"MAX km/u","% 6.1f", this->Speed->getPrevMaxSpeed());
    }


    void show_totals() {
        int Odo = this->Speed->getTotalDistance();
        int Quest = Odo + BIKE_DISTANCE_START;
        this->show_item_float(1,"ME","% 6.0f",Odo);
        this->show_item_float(2,"BIKE","% 6.0f",Quest);
    }

    void off() {
        OLED.clearDisplay();
        this->setDisplayModeHome();
        this->show_battery(true);
        this->setContrast(1);
        OLED.display();
    }

    void setContrast(byte contrast) {
        OLED.ssd1306_command(SSD1306_SETCONTRAST);
        OLED.ssd1306_command(contrast);
    }

    void show_item_string(int row, const char label[], const char value[] ) {
        int xl = 0;
        int xv = SCREEN_HALF_WIDTH - 44;
        int y = 14 + (row-1) * (SCREEN_HALF_HEIGHT-3);
        OLED.setTextSize(1);
        OLED.setCursor(xl, y+13);
        OLED.print(label);
        OLED.setTextSize(3);
        OLED.setCursor(xv, y);
        OLED.print(value);
    }

    void show_item_float(int row, const char label[], std::string format, float value ) {
        char valueStr[6];
        snprintf(valueStr, 7, format.c_str(), value);
        this->show_item_string(row,label,valueStr);
    }

    void show_item_time(int row, const char label[], unsigned long millis ) {
        char timeStr[7];
        unsigned long secs = millis / 1000;
        unsigned int minutes = secs / 60;
        if (minutes > 59) {
            unsigned int hours = secs / 3600;
            minutes = secs % 3600;
            snprintf(timeStr, 7, " % 2u.%02u", hours, minutes);
        }
        else {
            unsigned int seconds = secs % 60;
            snprintf(timeStr, 7, " % 2u:%02u", minutes, seconds);
        }
        this->show_item_string(row,label,timeStr);
    }

    void show_mode(const char mode[], int width = 5, bool inverse = false ) {
        CRGB menuColor = WHITE;
        CRGB menuTextColor = BLACK;
        CRGB textColor = WHITE;
        if ( inverse ) {
            OLED.fillScreen(WHITE);
            menuColor = BLACK;
            menuTextColor = WHITE;
            textColor = BLACK;
        }
        int currentMode = this->displayMode - 2;
        if ( this->settingsMenu ) {
            currentMode = this->displayMode - 10;
        }
        int x = currentMode * (SCREEN_WIDTH / width);
        int y = 0;
        int w = SCREEN_WIDTH / width + 5;
        int h = 9;
        int tx = x+2;

        OLED.fillRect(x,y,w,h,menuColor);
        OLED.drawLine(0,y+h-1,SCREEN_WIDTH,y+h-1,menuColor);
        OLED.setCursor(tx,y+1);
        OLED.setTextSize(1);
        OLED.setTextColor(menuTextColor);
        OLED.print(mode);
        OLED.setTextColor(textColor);
    }

    void showSettingsMode(const char mode[]) {
        this->show_mode( mode, 3, true);
    }

    void moveCursor( int inc ) {
        this->cursorPosition += inc;
        int maxCursorPosition = 4;
        if ( this->displayMode == DISPLAY_SETTINGS_TOTAL ) {
            if ( Speed->getTotalDistance() > 10000 ) {
                maxCursorPosition = 5;
            }
            if ( Speed->getTotalDistance() > 100000 ) {
                maxCursorPosition = 6;
            }
            if ( Speed->getTotalDistance() > 1000000 ) {
                maxCursorPosition = 7;
            }
        }
        if ( this->cursorPosition < 0 ) this->cursorPosition = 0;
        if ( this->cursorPosition > maxCursorPosition ) this->cursorPosition = maxCursorPosition;
    }

    void resetCursor() {
        this->cursorPosition = 0;
    }

    int cursorAmount() {
        int amount = 1;
        switch (cursorPosition)
        {
            case 1:
                amount = 10;
                break;
            case 2:
                amount = 100;
                break;
            case 3:
                amount = 1000;
                break;
            case 4:
                amount = 10000;
                break;
            case 5:
                amount = 100000;
                break;
        }
        return amount;
    }

    void showCursor() {
        int w = 18;
        int h = 4;
        int x = SCREEN_WIDTH - 1 - this->cursorPosition * w;
        int y = SCREEN_HALF_HEIGHT + 6;
        OLED.fillRect( x-w, y, w,h, BLACK);
    }

    void showResetTrip() {
        std::string format = "% 6.2f";
        if ( this->Speed->getTripDistance() >= 100 ) {
            format = "% 6.1f";
        }
        this->show_item_float(1,"TRIP",format,this->Speed->getTripDistance() );
        this->show_item_string(2,"RESET => Press UP","");
    }

    void showTotalEdit() {
        int Odo = this->Speed->getTotalDistance();
        this->show_item_float(1,"ME","% 6.0f",Odo);
        this->showCursor();
        this->show_item_string(2,"LEFT/RIGHT - UP/DOWN","");
    }

    void showTyreEdit() {
        int wheelNr = this->Speed->getClosestETRTO();
        this->show_item_float(1,Wheels[wheelNr].name, "% 6.0f", this->Speed->getWheelCircumference() * 1000 );
        this->showCursor();
        this->show_item_string(2,"LEFT/RIGHT - UP/DOWN","");
    }

    void show()
    {
        OLED.clearDisplay();
        this->show_indicators();

        if ( this->settingsMenu ) {
            switch (this->displayMode)
            {
                case DISPLAY_SETTINGS_TRIP:
                    this->showSettingsMode("TRIP");
                    this->showResetTrip();
                    break;
                case DISPLAY_SETTINGS_TYRE:
                    this->showSettingsMode("TYRE");
                    this->showTyreEdit();
                    break;
                case DISPLAY_SETTINGS_TOTAL:
                    this->showSettingsMode("TOTAL");
                    this->showTotalEdit();
                    break;
            }
        }
        else {
            switch (this->displayMode)
            {
                case DISPLAY_WELCOME :
                    this->show_welcome();
                    break;
                case DISPLAY_HOME:
                    this->show_speed();
                    this->show_time();
                    this->show_lights();
                    this->show_battery();
                    break;
                case DISPLAY_TODAY:
                    this->show_mode("DIST");
                    this->show_today_distances();
                    break;
                case DISPLAY_SPEEDS:
                    this->show_mode("SPED");
                    this->show_today_speeds();
                    break;
                case DISPLAY_TIME:
                    this->show_mode("TIME");
                    this->show_today_time();
                    break;
                case DISPLAY_PREV_DIST:
                    this->show_mode("DST<");
                    this->show_prev_dist();
                    break;
                case DISPLAY_PREV_SPEED:
                    this->show_mode("SPD<");
                    this->show_prev_speed();
                    break;
                case DISPLAY_TOTALS:
                    this->show_mode("ALL");
                    this->show_totals();
                    break;
            }
        }


        if ( this->Battery->isVeryLow() || this->Lights->getLights() >= LIGHTS_NORMAL ) {
            this->setContrast(1);
        }
        else {
            this->setContrast(128);
        }

        OLED.display();
    }

    void toggleSettings() {
        this->settingsMenu = ! this->settingsMenu;
        if ( this->settingsMenu ) {
            this->displayMode = DISPLAY_SETTINGS_TRIP;
            this->cursorPosition = 0;
        }
        else {
            this->Speed->storeMemory();
            this->resetDisplayMode();
        }
    }

    bool isSettingsMenu() {
        return this->displayMode >= DISPLAY_SETTINGS_TRIP;
    }

    bool isResetTripMenu() {
        return this->displayMode == DISPLAY_SETTINGS_TRIP;
    }

    bool isSetTyreMenu() {
        return this->displayMode == DISPLAY_SETTINGS_TYRE;
    }

    bool isSetTotalMenu() {
        return this->displayMode == DISPLAY_SETTINGS_TOTAL;
    }


};
