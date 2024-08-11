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
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// #if TEST
Adafruit_SSD1306 OLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, -1);  // on test board, we use second i2c port on pins 16 & 17
// #else
// Adafruit_SSD1306 OLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
// #endif

#define DISPLAY_DIMMED_LOOP_COUNT 2

enum DisplayType
{
    settings,
    three,
    four,
};

class DisplayClass
{

private:
    int displayMode = DISPLAY_HOME;
    int firstMode = DISPLAY_HOME;
    int lastMode = DISPLAY_TOTALS;
    int firstSettingsMode = DISPLAY_SETTINGS_TRIP;
    int lastSettingsMode = DISPLAY_SETTINGS_TOTAL;
    bool settingsMenu = false;
    bool startupQuestion = false;
    int cursorPosition = 0;

    SpeedClass *Speed;
    BatteryClass *Battery;
    IdleClass *IdleTimer;
    IndicatorClass *Indicators;
    LightsClass *Lights;

public:
    void init(SpeedClass *speed, BatteryClass *battery, IdleClass *idle, IndicatorClass *indicators, LightsClass *lights)
    {
        this->Speed = speed;
        this->Battery = battery;
        this->IdleTimer = idle;
        this->Indicators = indicators;
        this->Lights = lights;
        // Address 0x3D for 128x64
        if (!OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C))
        {
            Serial.println(F("SSD1306 allocation failed"));
            for (;;)
            {
                ;
            }
        }
        OLED.clearDisplay();
        OLED.setTextColor(WHITE);
    }

    void setDisplayModeHome()
    {
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
        if (this->settingsMenu)
        {
            if (this->displayMode > this->lastSettingsMode)
            {
                this->displayMode = this->firstSettingsMode;
            }
        }
        else
        {
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

    void askStartupQuestion()
    {
        this->startupQuestion = true;
    }

    void show_welcome()
    {
        OLED.setTextSize(2);
        OLED.setCursor(0, 0);
        OLED.print("Quest 631");
        OLED.drawLine(0, 20, SCREEN_WIDTH, 20, WHITE);

        if (this->startupQuestion)
        {
            OLED.setTextSize(1);
            OLED.setCursor(0, SCREEN_HALF_HEIGHT);
            OLED.print("Keep Distance ?");
            OLED.setCursor(0, SCREEN_HALF_HEIGHT + 20);
            OLED.print("YES => Press PLUS");
        }
    }

    void show_test()
    {
        OLED.setTextSize(1);
        OLED.setCursor(103,39);
        OLED.print("T");
    }

    void show_graphics_home() {
        OLED.drawCircle(SCREEN_HALF_WIDTH-1, SCREEN_HALF_HEIGHT, 37, WHITE);
        OLED.fillRect(0,-1, SCREEN_WIDTH, 17, BLACK);
        OLED.fillRect(0,SCREEN_HEIGHT - 17, SCREEN_WIDTH, 17, BLACK);

        OLED.drawCircle(SCREEN_HALF_WIDTH-1, SCREEN_HEIGHT - 8, 17, WHITE);
        OLED.fillRect(SCREEN_HALF_WIDTH - 20,SCREEN_HEIGHT - 36, 40, 21, BLACK);
    }

    void show_speed()
    {
        OLED.setTextColor(WHITE);
        if (this->Speed->isStarted())
        {

            float speed = this->Speed->getSpeed();
            int decis = (int)speed;
            int precision = (speed - decis) * 10;

            int x = SCREEN_HALF_WIDTH - 36;
            int y = 44;

            // Big
            OLED.setFont(&FreeMonoBold24pt7b);
            char speedStr[3];
            snprintf(speedStr, 3, "%2i", decis);
            OLED.setCursor(x, y);
            OLED.print(speedStr);
            // Small digit
            OLED.setFont(&FreeMonoBold12pt7b);
            snprintf(speedStr, 3, "%1i", precision);
            OLED.setCursor(x + 56, y);
            OLED.print(speedStr);
            // Sensor
            // OLED.setTextSize(1);
            // OLED.setCursor(x + 54, y+27);
            // if (this->Speed->getSpeedSensor())
            // {
            //     OLED.print(".");
            // }
            // else
            // {
            //     OLED.print(" ");
            // }

            // Faster / Slower than average | flash every second
            if (!this->Speed->isPaused() && (millis() / 500) % 2 == 0)
            {
                x = SCREEN_HALF_WIDTH + 22;
                y = 20;
                int w = 10;
                int h = 8;
                if (this->Speed->isFaster())
                {
                    OLED.fillTriangle(x, y + h, x + w / 2, y, x + w, y + h, WHITE);
                }
                else
                {
                    OLED.fillTriangle(x, y, x + w / 2, y + h, x + w, y, WHITE);
                }
            }
            OLED.setFont();
        }
    }

    void show_avg_max() {
        if (this->Speed->getAvgSpeed()>0) {
            char avStr[6];
            snprintf(avStr, 7, "%-6.1f", this->Speed->getAvgSpeed());
            OLED.setFont(&FreeSans9pt7b);
            OLED.setCursor(0,62);
            OLED.print(avStr);
        }
        if (this->Speed->getCurrentMaxSpeed()>0) {
            char maxStr[6];
            snprintf(maxStr, 7, "%6.1f", this->Speed->getCurrentMaxSpeed());
            OLED.setCursor(83,62);
            OLED.print(maxStr);
            OLED.setFont();
        }
    }

    void show_cadans() {
        if (this->Speed->getCadans() > 0) {
            char cadansStr[4];
            snprintf(cadansStr, 4, "%3i", this->Speed->getCadans());
            OLED.setFont(&FreeSans9pt7b);
            OLED.setCursor(48,62);
            OLED.print(cadansStr);
            OLED.setFont();
        }
    }

    void show_indicators()
    {
        int x = SCREEN_HALF_WIDTH - 19;
        int y = 1;
        int size = 5;
        if (this->Indicators->getStateLeft())
        {
            OLED.fillTriangle(x, y + size, x + size, y, x+ size, y + 2 * size, WHITE);
        }
        if (this->Indicators->getStateRight())
        {
            x = SCREEN_HALF_WIDTH + 20;
            OLED.fillTriangle(x - size - 2, y, x - 2, y + size, x - size - 2, y + 2 * size, WHITE);
        }
    }

    void show_time()
    {
        int x = SCREEN_HALF_WIDTH + 18;
        int y = 12;
        char hourStr[3];
        char minStr[3];
        if (this->IdleTimer->warning())
        {
            // Idle clock
            snprintf(minStr, 3, "%02i", this->IdleTimer->remainingSeconds());
        }
        else
        {
            // Normal clock
            snprintf(hourStr, 3, "%2i", hour());
            snprintf(minStr, 3, "%02i", minute());
        }

        OLED.setTextSize(1);
        OLED.setFont(&FreeSans9pt7b);
        if (!this->IdleTimer->warning())
        {
            OLED.setCursor(x, y);
            OLED.print(hourStr);
        }
        OLED.setCursor(x + 25, y);
        OLED.print(minStr);

        if (!this->IdleTimer->warning())
        {
            OLED.setCursor(x + 21, y-1);
            if ((millis() / 500) % 2 == 0)
            {
                OLED.print(":");
            }
            else
            {
                OLED.print(" ");
            }
        }
        OLED.setFont();
    }

    void show_distance()
    {
        if (this->Speed->getCurrentDistance()>0) {
            int x = 0;
            int y = 12;
            std::string format = "%-6.2f";
            if (this->Speed->getCurrentDistance() >= 100)
            {
                format = "%-6.1f";
            }
            char distStr[9];
            snprintf(distStr, 10, format.c_str(), this->Speed->getCurrentDistance());
            OLED.setTextSize(1);
            OLED.setFont(&FreeSans9pt7b);
            OLED.setCursor(x, y);
            OLED.print(distStr);
            OLED.setFont();
        }
    }

    void show_battery(bool off = false)
    {
        OLED.setTextColor(WHITE);
        int toppad = 3;
        int innerpad = 2;
        int x = 111;
        int y = 18;
        int w = 16;
        int h = 27;
        OLED.drawRect(x + toppad, y, w - 2 * toppad, toppad, WHITE);
        OLED.drawRect(x, y + toppad - 1, w, h, WHITE);

        if (!off)
        {
            float percentage = this->Battery->getBatteryPercentage();
            int juiceHeight = (h - innerpad * 2) * percentage;
            int color = WHITE;
            if (this->Battery->isLow()) {
                if ((millis() / 333) % 2 == 0)
                {
                    color = BLACK;
                }
            }
            OLED.fillRect(x + innerpad, y + innerpad + (h - juiceHeight) - 2, w - 2 * innerpad, juiceHeight, color);

            OLED.drawLine(x + innerpad, y + 1 + innerpad + h / 4, x + w - innerpad, y + 1 + innerpad + h / 4, BLACK);
            OLED.drawLine(x + innerpad, y + 0 + innerpad + h / 2, x + w - innerpad, y + 0 + innerpad + h / 2, BLACK);
            OLED.drawLine(x + innerpad, y + 1 + innerpad + h / 4 * 3, x + w - innerpad, y + 1 + innerpad + h / 4 * 3, BLACK);

            // print cell voltage
            // int cellMilliV = this->Battery->getCellVoltage();
            // int cellV = cellMilliV / 1000;
            // int cellM = (cellMilliV - (1000 * cellV)) / 10;
            // char cellMstr[4];
            // snprintf(cellMstr, 3, "%02i", cellM);

            // OLED.setTextSize(2);
            // OLED.setCursor(x + 20, y + 10 + toppad);
            // OLED.print(cellV);
            // OLED.setTextSize(1);
            // OLED.setCursor(x + 31, y + 17 + toppad);
            // OLED.print(cellMstr);

            // print percentage
            // if (percentage > 99)
            //     percentage = 99;
            // char percStr[5];
            // snprintf(percStr, 5, "%-i%%", percentage);
            // OLED.setTextSize(1);
            // OLED.setCursor(x + 20, y + toppad);
            // OLED.print(percStr);
            // OLED.print("%");
        }
        else
        {
            OLED.setCursor(x + w + 12, y + toppad);
            OLED.setTextSize(3);
            OLED.print(this->Battery->secondsUntilPowerOff());
        }
    }

    void show_lights()
    {

        // headlights
        int x = 0;
        int y = 17;
        int xr = x;
        int yr = y + 17;
        int w = 24;
        int h = 14;
        switch (this->Lights->getLights())
        {
        case LIGHTS_OFF:
            // OLED.drawBitmap(x, y, icoLowBeam, w, h, WHITE);
            break;
        case LIGHTS_DIM:
            OLED.drawBitmap(x, y, icoLowBeam, w, h, WHITE);
            break;
        case LIGHTS_ON:
            OLED.drawBitmap(x, y, icoDefaultBeam, w, h, WHITE);
            OLED.drawBitmap(xr, yr, icoLowRear, w, h, WHITE);
            break;
        case LIGHTS_NORMAL:
            OLED.drawBitmap(x, y, icoHighBeam, w, h, WHITE);
            OLED.drawBitmap(xr, yr, icoLowRear, w, h, WHITE);
            break;
        case LIGHTS_BEAM:
            OLED.drawBitmap(x, y, icoMaxBeam, w, h, WHITE);
            OLED.drawBitmap(xr, yr, icoLowRear, w, h, WHITE);
            break;
        case LIGHTS_FOG:
            OLED.drawBitmap(x, y, icoMaxBeam, w, h, WHITE);
            OLED.drawBitmap(xr, yr, icoHighRear, w, h, WHITE);
            break;
        }
        if (this->Lights->getBrake())
        {
            OLED.drawBitmap(SCREEN_HALF_WIDTH - 12, 0, icoBrakeRear, w, h, WHITE);
        }
    }

    void show_today_distances()
    {
        this->show_item_float(four, 1, "Dist", "%-6.1f", this->Speed->getCurrentDistance());
        this->show_item_float(four, 2, "Today", "% 6.1f", this->Speed->getDayDistance());
        this->show_item_float(four, 3, "Prev", "%-6.1f", this->Speed->getPrevDistance());
        this->show_item_float(four, 4, "Trip 1", "% 6.1f", this->Speed->getTripDistance());
    }

    void show_today_speeds()
    {
        this->show_item_float(four, 1, "Avg", "%-6.1f", this->Speed->getAvgSpeed());
        this->show_item_float(four, 2, "Max", "% 6.1f", this->Speed->getCurrentMaxSpeed());
        this->show_item_time(four, 3, "Time", this->Speed->getCurrentTime());
        this->show_item_time(four, 4, "Total", this->Speed->getTotalTime(), true);
    }

    // void show_trips() {
    //     this->show_item_float(four,1,"Tocht 1","%-6.1f",this->Speed->getTripDistance(1) );
    //     this->show_item_float(four,3,"Tocht 2","%-6.1f",this->Speed->getTripDistance(2) );
    //     this->show_item_float(four,4,"Tocht 3","% 6.1f",this->Speed->getTripDistance(3) );
    // }

    // void show_prev_speed() {
    //     this->show_item_float(three,1,"Avg","% 6.1f", this->Speed->getPrevAvgSpeed());
    //     this->show_item_float(three,2,"Max","% 6.1f", this->Speed->getPrevMaxSpeed());
    // }

    void show_totals()
    {
        int Odo = this->Speed->getTotalDistance();
        int Quest = Odo + BIKE_DISTANCE_START;
        this->show_item_float(four, 1, "Total", "%-6.0f", Odo);
        this->show_item_float(four, 2, "Quest", "% 6.0f", Quest);
        this->show_item_float(four, 3, "Trip 2", "%-6.0f", this->Speed->getTripDistance(2));
        this->show_item_float(four, 4, "Trip 3", "% 6.0f", this->Speed->getTripDistance(2));
    }

    void off()
    {
        OLED.clearDisplay();
        this->setDisplayModeHome();
        this->show_battery(true);
        OLED.display();
    }

    void show_item_string(DisplayType type, int row, const char label[], const char value[])
    {
        if (type == settings)
        {
            if (row == 1)
            {
                OLED.setTextSize(1);
                OLED.setCursor(1, 16);
                char labelStr[10];
                snprintf(labelStr, 11, "%s", label);
                OLED.print(labelStr);
                OLED.setTextSize(2);
                OLED.setCursor(SCREEN_HALF_WIDTH - 32, 10);
                char valueStr[8];
                snprintf(valueStr, 9, "%8s", value);
                OLED.print(valueStr);
            }
            if (row == 2)
            {
                OLED.setTextSize(1);
                OLED.setCursor(1, 28);
                char labelStr[10];
                snprintf(labelStr, 11, "%s", label);
                OLED.print(labelStr);
                OLED.setCursor(SCREEN_HALF_WIDTH + 16, 32);
                char valueStr[8];
                snprintf(valueStr, 9, "%8s", value);
                OLED.print(valueStr);
            }
            if (row == 3)
            {
                OLED.setTextSize(1);
                OLED.setCursor(1, 42);
                char labelStr[10];
                snprintf(labelStr, 11, "%s", label);
                OLED.print(labelStr);
                OLED.setCursor(SCREEN_HALF_WIDTH + 16, 42);
                char valueStr[8];
                snprintf(valueStr, 9, "%8s", value);
                OLED.print(valueStr);
            }

            if (row == 4)
            {
                OLED.setTextSize(1);
                OLED.setCursor(1, SCREEN_HEIGHT - 10);
                OLED.print(label);
                OLED.setTextSize(2);
                OLED.setCursor(1, SCREEN_HEIGHT - 10);
                OLED.print(value);
            }
        }

        if (type == three)
        {
            if (row == 1)
            {
                OLED.setTextSize(1);
                OLED.setCursor(SCREEN_HALF_WIDTH + 4, 0);
                char labelStr[10];
                snprintf(labelStr, 11, "%10s", label);
                OLED.print(labelStr);
                OLED.setTextSize(2);
                OLED.setCursor(SCREEN_HALF_WIDTH - 32, 10);
                char valueStr[8];
                snprintf(valueStr, 9, "%8s", value);
                OLED.print(valueStr);
            }
            if (row == 2)
            {
                OLED.setTextSize(1);
                OLED.setCursor(0, SCREEN_HALF_HEIGHT + 5);
                OLED.print(label);
                OLED.setTextSize(2);
                OLED.setCursor(0, SCREEN_HEIGHT - 16);
                OLED.print(value);
            }
            if (row == 3)
            {
                OLED.setTextSize(1);
                OLED.setCursor(SCREEN_HALF_WIDTH + 4, SCREEN_HALF_HEIGHT + 5);
                char labelStr[10];
                snprintf(labelStr, 11, "%10s", label);
                OLED.print(labelStr);
                OLED.setTextSize(2);
                OLED.setCursor(SCREEN_HALF_WIDTH - 32, SCREEN_HEIGHT - 16);
                char valueStr[8];
                snprintf(valueStr, 9, "%8s", value);
                OLED.print(valueStr);
            }
        }
        if (type == four)
        {
            if (row == 1)
            {
                OLED.setTextSize(1);
                OLED.setCursor(0, 0);
                OLED.print(label);
                OLED.setTextSize(2);
                OLED.setCursor(0, 10);
                OLED.print(value);
            }
            if (row == 2)
            {
                OLED.setTextSize(1);
                OLED.setCursor(SCREEN_HALF_WIDTH + 4, 0);
                char labelStr[10];
                snprintf(labelStr, 11, "%10s", label);
                OLED.print(labelStr);
                OLED.setTextSize(2);
                OLED.setCursor(SCREEN_HALF_WIDTH - 32, 10);
                char valueStr[8];
                snprintf(valueStr, 9, "%8s", value);
                OLED.print(valueStr);
            }
            if (row == 3)
            {
                OLED.setTextSize(1);
                OLED.setCursor(0, SCREEN_HALF_HEIGHT + 5);
                OLED.print(label);
                OLED.setTextSize(2);
                OLED.setCursor(0, SCREEN_HEIGHT - 16);
                OLED.print(value);
            }
            if (row == 4)
            {
                OLED.setTextSize(1);
                OLED.setCursor(SCREEN_HALF_WIDTH + 4, SCREEN_HALF_HEIGHT + 5);
                char labelStr[10];
                snprintf(labelStr, 11, "%10s", label);
                OLED.print(labelStr);
                OLED.setTextSize(2);
                OLED.setCursor(SCREEN_HALF_WIDTH - 32, SCREEN_HEIGHT - 16);
                char valueStr[8];
                snprintf(valueStr, 9, "%8s", value);
                OLED.print(valueStr);
            }
        }
    }

    void show_item_float(DisplayType type, int row, const char label[], std::string format, float value)
    {
        char valueStr[6];
        snprintf(valueStr, 7, format.c_str(), value);
        this->show_item_string(type, row, label, valueStr);
    }

    void show_item_time(DisplayType type, int row, const char label[], unsigned long millis, bool align_right = false)
    {
        char timeStr[8];
        unsigned long secs = millis / 1000;
        unsigned int minutes = secs / 60;
        if (minutes > 59)
        {
            unsigned int hours = secs / 3600;
            minutes = minutes % 60;
            snprintf(timeStr, 8, "%dh%02d", hours, minutes);
        }
        else
        {
            unsigned int seconds = secs % 60;
            snprintf(timeStr, 8, "%d:%02d", minutes, seconds);
        }
        this->show_item_string(type, row, label, timeStr);
    }

    void show_item_clock(DisplayType type, int row, const char label[], int hour, int min)
    {
        char clockStr[9];
        snprintf(clockStr, 8, " %2d:%02d", hour, min);
        this->show_item_string(type, row, label, clockStr);
    }

    void show_mode(const char mode[], int width = 5, bool inverse = false)
    {
        CRGB menuColor = WHITE;
        CRGB menuTextColor = BLACK;
        CRGB textColor = WHITE;
        if (inverse)
        {
            OLED.fillScreen(WHITE);
            menuColor = BLACK;
            menuTextColor = WHITE;
            textColor = BLACK;
        }
        int currentMode = this->displayMode - 2;
        if (this->settingsMenu)
        {
            currentMode = this->displayMode - 10;
        }
        int x = currentMode * (SCREEN_WIDTH / width);
        int y = 0;
        int w = SCREEN_WIDTH / width + 5;
        int h = 9;
        int tx = x + 3;

        OLED.fillRect(x, y, w, h, menuColor);
        OLED.drawLine(0, y + h - 1, SCREEN_WIDTH, y + h - 1, menuColor);
        OLED.setCursor(tx, y + 1);
        OLED.setTextSize(1);
        OLED.setTextColor(menuTextColor);
        OLED.print(mode);
        OLED.setTextColor(textColor);
    }

    void showSettingsMode(const char mode[])
    {
        this->show_mode(mode, 4, true);
    }

    void moveCursor(int inc)
    {
        this->cursorPosition += inc;
        int maxCursorPosition = 4;
        if (this->displayMode == DISPLAY_SETTINGS_TOTAL)
        {
            if (Speed->getTotalDistance() > 10000)
            {
                maxCursorPosition = 5;
            }
            if (Speed->getTotalDistance() > 100000)
            {
                maxCursorPosition = 6;
            }
            if (Speed->getTotalDistance() > 1000000)
            {
                maxCursorPosition = 7;
            }
        }
        if (this->cursorPosition < 0)
            this->cursorPosition = 0;
        if (this->cursorPosition > maxCursorPosition)
            this->cursorPosition = maxCursorPosition;
    }

    void resetCursor()
    {
        this->cursorPosition = 0;
    }

    int cursorAmount()
    {
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

    void showCursor()
    {
        int w = 12;
        int h = 3;
        int x = SCREEN_WIDTH - 1 - this->cursorPosition * w;
        int y = SCREEN_HALF_HEIGHT - 4;
        OLED.fillRect(x - w, y, w, h, BLACK);
    }

    void showResetTrip()
    {
        std::string format = "% 6.2f";
        if (this->Speed->getTripDistance() >= 100)
        {
            format = "% 6.1f";
        }
        this->show_item_float(settings, 1, "TRIP 1", format, this->Speed->getTripDistance(1));
        this->show_item_float(settings, 2, "TRIP 2", format, this->Speed->getTripDistance(2));
        this->show_item_float(settings, 3, "TRIP 3", format, this->Speed->getTripDistance(3));
        this->show_item_string(settings, 4, "RESET: UP DOWN LEFT", "");
    }

    void showSetTime()
    {
        this->show_item_clock(settings, 1, "Clock", hour(), minute());
        this->showCursor();
        this->show_item_string(settings, 4, "LEFT/RIGHT - UP/DOWN", "");
    }

    void showTotalEdit()
    {
        int Odo = this->Speed->getTotalDistance();
        this->show_item_float(settings, 1, "ME", "% 6.0f", Odo);
        this->showCursor();
        this->show_item_string(settings, 4, "LEFT/RIGHT - UP/DOWN", "");
    }

    void showTyreEdit()
    {
        int wheelNr = this->Speed->getClosestETRTO();
        this->show_item_float(settings, 1, Wheels[wheelNr].name, "% 6.0f", this->Speed->getWheelCircumference() * 1000);
        this->showCursor();
        this->show_item_string(settings, 4, "LEFT/RIGHT - UP/DOWN", "");
    }

    void show()
    {
        OLED.clearDisplay();
        bool dimmed = (this->Lights->getLights() > LIGHTS_ON) || this->Battery->isLow();
        this->dim(dimmed);

        if (this->settingsMenu)
        {
            switch (this->displayMode)
            {
            case DISPLAY_SETTINGS_TRIP:
                this->showSettingsMode("TRIP");
                this->showResetTrip();
                break;
            case DISPLAY_SETTINGS_CLOCK:
                this->showSettingsMode("CLOCK");
                this->showSetTime();
                break;
            case DISPLAY_SETTINGS_TYRE:
                this->showSettingsMode("TYRE");
                this->showTyreEdit();
                break;
            case DISPLAY_SETTINGS_TOTAL:
                this->showSettingsMode("BIKE");
                this->showTotalEdit();
                break;
            }
        }
        else
        {
            switch (this->displayMode)
            {
            case DISPLAY_WELCOME:
                this->show_welcome();
                break;
            case DISPLAY_HOME:
                this->show_graphics_home();
                if (!this->IdleTimer->warning())
                {
                    this->show_speed();
                    this->show_distance();
                    this->show_avg_max();
                    this->show_cadans();
                    this->show_lights();
                    this->show_battery();
                    this->show_indicators();

                    #if TEST
                    this->show_test();
                    #endif
                }
                this->show_time();
                break;
            case DISPLAY_TODAY:
                this->show_today_distances();
                break;
            case DISPLAY_SPEEDS:
                this->show_today_speeds();
                break;
            // case DISPLAY_TRIPS:
            //     this->show_trips();
            //     break;
            case DISPLAY_TOTALS:
                this->show_totals();
                break;
            }
        }
        OLED.display();
    }

    void toggleSettings()
    {
        this->settingsMenu = !this->settingsMenu;
        if (this->settingsMenu)
        {
            this->displayMode = DISPLAY_SETTINGS_TRIP;
            this->cursorPosition = 0;
        }
        else
        {
            this->Speed->storeMemory();
            this->resetDisplayMode();
        }
    }

    bool isSettingsMenu()
    {
        return this->displayMode >= DISPLAY_SETTINGS_TRIP;
    }

    bool isResetTripMenu()
    {
        return this->displayMode == DISPLAY_SETTINGS_TRIP;
    }

    bool isSetClockMenu()
    {
        return this->displayMode == DISPLAY_SETTINGS_CLOCK;
    }

    bool isSetTyreMenu()
    {
        return this->displayMode == DISPLAY_SETTINGS_TYRE;
    }

    bool isSetTotalMenu()
    {
        return this->displayMode == DISPLAY_SETTINGS_TOTAL;
    }

    void dim(bool dim = true)
    {
        OLED.dim(dim);
    }
};
