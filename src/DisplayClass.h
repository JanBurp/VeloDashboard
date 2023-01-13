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
        this->displayMode++;
        if (this->displayMode > this->lastMode)
        {
            this->displayMode = this->firstMode;
        }
    }

    void prevDisplayMode()
    {
        this->displayMode--;
        if (this->displayMode < this->firstMode)
        {
            this->displayMode = this->firstMode;
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
            }

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

    void show_distances()
    {
        this->show_item_float(1,"dist","%-6.2f",this->Speed->getDistance());
        this->show_item_string(2,"day","0");
        this->show_item_string(3,"trip","0");
    }

    void show_speeds()
    {
        this->show_item_float(1,"avg","%-4.1f", this->Speed->getAvgSpeed());
        this->show_item_float(2,"max","%-4.1f", this->Speed->getMaxSpeed());
        this->show_item_time(3,"time", this->Speed->getTripTime());
    }

    void show_prev() {
        this->show_item_string(1,"prev avg","-");
        this->show_item_string(2,"prev max","-");
        this->show_item_string(3,"prev dst","-");
    }

    void show_totals() {
        int Odo = this->Speed->getOdoDistance();
        int Quest = Odo + BIKE_DISTANCE_START;
        this->show_item_float(1,"me","%-6.0f",Odo);
        this->show_item_float(2,"quest","%-6.0f",Quest);
        this->show_item_float(3,"tyre", "%-1.4f", Wheels[WheelNumber].circumference);
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
        int x = 0;
        int y = (row-1) * SCREEN_HALF_HEIGHT + 6;
        if (row==3) {
            x = SCREEN_HALF_WIDTH + 4;
            y = SCREEN_HALF_HEIGHT + 6;
        }
        OLED.setTextSize(1);
        OLED.setCursor(x, y);
        OLED.print(label);
        OLED.setTextSize(2);
        OLED.setCursor(x, y+12);
        OLED.print(value);
    }

    void show_item_float(int row, const char label[], const char format[], float value ) {
        char valueStr[6];
        snprintf(valueStr, 7, format, value);
        this->show_item_string(row,label,valueStr);
    }

    void show_item_time(int row, const char label[], unsigned long millis ) {
        char timeStr[7];
        unsigned long secs = millis / 1000;
        unsigned int minutes = secs / 60;
        unsigned int seconds = secs % 60;
        snprintf(timeStr, 7, "%u:%02u", minutes, seconds);
        this->show_item_string(row,label,timeStr);
    }

    // void show_mode() {
    //     int currentMode = this->displayMode - 2;
    //     int x = 0;
    //     int y = currentMode * (SCREEN_HEIGHT / 4);
    //     int w = 1;
    //     int h = SCREEN_HEIGHT / 4;
    //     OLED.drawRect(x,y,w,h,WHITE);
    // }

    void show()
    {
        OLED.clearDisplay();

        if (this->displayMode == DISPLAY_WELCOME)
        {
            this->show_welcome();
        }
        else
        {

            this->show_speed();
            this->show_indicators();

            switch (this->displayMode)
            {
                case DISPLAY_HOME:
                    this->show_time();
                    this->show_lights();
                    this->show_battery();
                    break;
                case DISPLAY_DISTANCE:
                    // this->show_mode();
                    this->show_distances();
                    break;
                case DISPLAY_SPEEDS:
                    // this->show_mode();
                    this->show_speeds();
                    break;
                case DISPLAY_PREV:
                    // this->show_mode();
                    this->show_prev();
                    break;
                case DISPLAY_TOTALS:
                    // this->show_mode();
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
};
