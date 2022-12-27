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
    int displayMode = DISPLAY_SPEED_AND_TIME;
    int firstMode = DISPLAY_SPEED_AND_TIME;
    int lastMode = DISPLAY_TRIPTIME;
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
        this->displayMode = 0;
    }

    void _show_welcome()
    {
        OLED.setTextSize(2);
        OLED.setCursor(0, 0);
        OLED.print("Quest 631");

        OLED.setCursor(0, 32);
        OLED.print(Wheels[WheelNumber].name);
        OLED.setCursor(0, 48);
        OLED.print(Wheels[WheelNumber].circumference);
    }

    void _show_speed()
    {
        if ( this->Speed->isStarted() ) {

            // Show distance & average when paused
            if ( this->Speed->isPaused() ) {
                char avgSpeedStr[11];
                snprintf(avgSpeedStr, 11, "%-3.1f", this->Speed->getAvgSpeed());
                char distStr[13];
                snprintf(distStr, 13, "%-3.1f", this->Speed->getDistance());

                OLED.setTextSize(1);
                OLED.setCursor(52, 6 );
                OLED.print("AVG");
                OLED.setCursor(52, 30 );
                OLED.print("DST");

                OLED.setTextSize(2);
                OLED.setCursor(80, 0 );
                OLED.print(avgSpeedStr);
                OLED.setCursor(80, 24 );
                OLED.print(distStr);
            }

            // Show speeed
            else {
                float speed = this->Speed->getSpeed();
                int decis = (int)speed;
                int precision = (speed - decis) * 10;

                OLED.setTextSize(5);
                char speedStr[3];
                snprintf(speedStr, 3, "%2i", decis);
                OLED.setCursor(38, 0);
                OLED.print(speedStr);
                snprintf(speedStr, 3, "%1i", precision);
                OLED.setTextSize(3);
                OLED.setCursor(110, 14);
                OLED.print(speedStr);

                // Sensor
                OLED.setTextSize(2);
                OLED.setCursor(96, 22);
                if (this->Speed->getSpeedSensor())
                {
                    OLED.print(".");
                }
                else
                {
                    OLED.print(" ");
                }

                // Faster / Slower than average
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

    void _show_indicators()
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

    void _show_time()
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

    void _show_battery( bool off = false)
    {
        int toppad = 4;
        int innerpad = 2;
        int x = 1;
        int y = SCREEN_HALF_HEIGHT_INFO - 2;
        int w = 20;
        int h = 22;
        OLED.drawRect(x + toppad, y, x + w - 2 * toppad, toppad, WHITE);
        OLED.drawRect(x, y + toppad, x + w, h, WHITE);

        if ( !off ) {
            float percentage = this->Battery->getBatteryPercentage() / 100.0;
            int juiceHeight = (h - innerpad * 2) * percentage;
            OLED.fillRect(x + innerpad, y + innerpad + (h - juiceHeight), x + w - 2 * innerpad, juiceHeight, WHITE);

            // print cell voltage
            int cellMilliV = this->Battery->getCellVoltage();
            // float cellV = cellMilliV / 1000.0;
            int cellV = cellMilliV / 1000;
            int cellM = (cellMilliV - (1000*cellV))/10;
            OLED.setTextSize(2);
            OLED.setCursor(x + w + 4, y + toppad);
            OLED.print( cellV );
            OLED.setTextSize(1);
            OLED.setCursor(x + w + 4, y + toppad + 15);
            OLED.print( cellM );

            // print percentage
            OLED.setCursor(x-6, y + toppad + 4);
            OLED.setTextSize(1);
            OLED.print(percentage);

            // // LEDstrips current use
            // x = x + w + 4;
            // y = y + 4;
            // h = h;
            // w = 4;
            // int ledMilliAmps = this->LEDstrips->max_used_milliamps();
            // if (TEST) {
            //     percentage = (ledMilliAmps / 1000.0);
            // }
            // else {
            //     percentage = (ledMilliAmps / 2000.0);
            // }
            // // OLED.setCursor(x + 10,y);
            // // OLED.setTextSize(1);
            // // OLED.print(ledMilliAmps);
            // OLED.drawRect(x, y, w, h,WHITE);
            // OLED.fillRect(x,y + h - (percentage*h),w,h,WHITE);
        }
        else {
            OLED.setCursor(x + w + 12, y + toppad);
            OLED.setTextSize(3);
            OLED.print(this->Battery->secondsUntilPowerOff());
        }
    }

    void _show_lights()
    {
        // headlights
        int x = 0;
        int y = 0;
        int w = 24;
        int h = 14;
        switch (this->Lights->getLights())
        {
        case LIGHTS_OFF:
            OLED.drawBitmap(x, y, icoLowBeam, w, h, WHITE);
            break;
        case LIGHTS_DIM:
            OLED.drawBitmap(x, y, icoDefaultBeam, w, h, WHITE);
            break;
        case LIGHTS_NORMAL:
            OLED.drawBitmap(x, y, icoHighBeam, w, h, WHITE);
            break;
        case LIGHTS_BEAM:
            OLED.drawBitmap(x, y, icoMaxBeam, w, h, WHITE);
            break;
        }
        if (this->Lights->getHorn()) {
            OLED.setCursor(x + w, y);
            OLED.setTextSize(2);
            OLED.print("!");
        }

        // rear
        y = 20;
        if (this->Lights->getBrake())
        {
            OLED.drawBitmap(x, y, icoBrakeRear, w, h, WHITE);
        }
        else {
            switch (this->Lights->getBackLights()) {
            case BACKLIGHTS_DIM:
                OLED.drawBitmap(x, y, icoLowRear, w, h, WHITE);
                break;
            case BACKLIGHTS_NORMAL:
                OLED.drawBitmap(x, y, icoHighRear, w, h, WHITE);
                break;
            case BACKLIGHTS_FOG:
                OLED.drawBitmap(x, y, icoFogRear, w, h, WHITE);
                break;
            }
        }

    }

    void _show_speeds()
    {
        char avgSpeedStr[6];
        snprintf(avgSpeedStr, 6, "%-4.1f", this->Speed->getAvgSpeed());
        char maxSpeedStr[6];
        snprintf(maxSpeedStr, 6, "%4.1f", this->Speed->getMaxSpeed());

        OLED.setTextSize(1);
        OLED.setCursor(0, SCREEN_HALF_HEIGHT_INFO);
        OLED.print("avg");
        OLED.setCursor(SCREEN_WIDTH - 20, SCREEN_HALF_HEIGHT_INFO);
        OLED.print("max");

        OLED.setTextSize(2);
        OLED.setCursor(0, SCREEN_HALF_HEIGHT_VALUES);
        OLED.print(avgSpeedStr);
        OLED.setCursor(80, SCREEN_HALF_HEIGHT_VALUES);
        OLED.print(maxSpeedStr);
    }

    void _show_distances()
    {
        char distStr[8];
        snprintf(distStr, 8, "%-6.2f", this->Speed->getDistance());
        char odoStr[8];
        snprintf(odoStr, 8, "%5lu", this->Speed->getOdoDistance());

        OLED.setTextSize(1);
        OLED.setCursor(0, SCREEN_HALF_HEIGHT_INFO);
        OLED.print("dist");
        OLED.setCursor(SCREEN_WIDTH - 20, SCREEN_HALF_HEIGHT_INFO);
        OLED.print("odo");

        OLED.setTextSize(2);
        OLED.setCursor(0, SCREEN_HALF_HEIGHT_VALUES);
        OLED.print(distStr);
        OLED.setCursor(68, SCREEN_HALF_HEIGHT_VALUES);
        OLED.print(odoStr);
    }

    void _show_triptime()
    {
        char timeTripStr[7];
        unsigned long tripTimeMs = this->Speed->getTripTime();
        unsigned long tripTimeSec = tripTimeMs / 1000;
        unsigned int minutes = tripTimeSec / 60;
        unsigned int seconds = tripTimeSec % 60;
        snprintf(timeTripStr, 7, "%2u:%02u", minutes, seconds);

        // char usedMilliAmps[5];
        // unsigned int milliAmps = LEDstrips.max_used_milliamps();
        // snprintf(usedMilliAmps, 5, "%4u", milliAmps );

        OLED.setTextSize(1);
        OLED.setCursor(0, SCREEN_HALF_HEIGHT_INFO);
        OLED.print("trip time");
        // OLED.setCursor(SCREEN_WIDTH - 16, SCREEN_HALF_HEIGHT_INFO);
        // OLED.print("mA");

        OLED.setTextSize(2);
        OLED.setCursor(0, SCREEN_HALF_HEIGHT_VALUES);
        OLED.print(timeTripStr);
        // OLED.setCursor(80, SCREEN_HALF_HEIGHT_VALUES);
        // OLED.print(usedMilliAmps);
    }

    void off() {
        OLED.clearDisplay();
        this->_show_battery(true);
        this->setContrast(1);
        OLED.display();
    }

    void setContrast(byte contrast) {
        OLED.ssd1306_command(SSD1306_SETCONTRAST);
        OLED.ssd1306_command(contrast);
    }

    void show()
    {
        OLED.clearDisplay();

        if (this->displayMode == DISPLAY_WELCOME)
        {
            this->_show_welcome();
        }
        else
        {
            this->_show_speed();
            this->_show_indicators();
            switch (this->displayMode)
            {
            case DISPLAY_SPEED_AND_TIME:
                this->_show_battery();
                this->_show_lights();
                this->_show_time();
                break;
            case DISPLAY_SPEEDS:
                this->_show_lights();
                this->_show_speeds();
                break;
            case DISPLAY_DISTANCE:
                this->_show_lights();
                this->_show_distances();
                break;
            case DISPLAY_TRIPTIME:
                this->_show_lights();
                this->_show_triptime();
                break;
            }
        }

        if (this->Battery->isVeryLow()) {
            this->setContrast(64);
        }

        OLED.display();
    }
};
