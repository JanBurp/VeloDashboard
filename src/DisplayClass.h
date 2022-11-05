#pragma once

#include "Arduino.h"
#include "settings.h"
#include "SpeedClass.h"
#include "IndicatorClass.h"
#include "LightsClass.h"

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
    IndicatorClass *Indicators;
    LightsClass *Lights;

public:
    void init(SpeedClass *speed, IndicatorClass *indicators, LightsClass *lights)
    {
        this->Speed = speed;
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
        if (!this->Speed->isPaused())
        {
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
        snprintf(hourStr, 3, "%2i", hour());
        char minStr[3];
        snprintf(minStr, 3, "%02i", minute());

        OLED.setTextSize(3);
        OLED.setCursor(49, SCREEN_HALF_HEIGHT_VALUES - 6);
        OLED.print(hourStr);
        OLED.setCursor(92, SCREEN_HALF_HEIGHT_VALUES - 6);
        OLED.print(minStr);

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

    void _show_battery()
    {
        int toppad = 4;
        int innerpad = 2;
        int x = 1;
        int y = SCREEN_HALF_HEIGHT_INFO - 2;
        int w = 20;
        int h = 22;
        OLED.drawRect(x + toppad, y, x + w - 2 * toppad, toppad, WHITE);
        OLED.drawRect(x, y + toppad, x + w, h, WHITE);
        float percentage = 0.5;
        int juiceHeight = (h - innerpad * 2) * percentage;
        OLED.fillRect(x+innerpad, y + innerpad + (h - juiceHeight), x + w - 2 * innerpad, juiceHeight, WHITE);
        OLED.setCursor(x + w, y + toppad);
        OLED.setTextSize(3);
        if (percentage <= 0.15)
        {
            OLED.print("!");
        }
        if (percentage <= 0.1)
        {
            OLED.setCursor(x + w + 4, y + toppad);
            OLED.print("!");
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

        // rear
        // x = 26;
        y = 20;
        switch (this->Lights->getLights())
        {
        case LIGHTS_OFF:
            OLED.drawBitmap(x, y, icoLowRear, w, h, WHITE);
            break;
        case LIGHTS_DIM:
            OLED.drawBitmap(x, y, icoLowRear, w, h, WHITE);
            break;
        case LIGHTS_NORMAL:
            OLED.drawBitmap(x, y, icoHighRear, w, h, WHITE);
            break;
        case LIGHTS_BEAM:
            OLED.drawBitmap(x, y, icoBrakeRear, w, h, WHITE);
            break;
        }

        // OLED.drawLine(26,SCREEN_HALF_HEIGHT_INFO,24,SCREEN_HALF_HEIGHT_INFO+2,WHITE);
        // OLED.drawLine(24,SCREEN_HALF_HEIGHT_INFO+2,24,SCREEN_HALF_HEIGHT_INFO+4,WHITE);
        // OLED.drawLine(24,SCREEN_HALF_HEIGHT_INFO+4,26,SCREEN_HALF_HEIGHT_INFO+6,WHITE);
        // OLED.fillCircle(32,SCREEN_HALF_HEIGHT_INFO + 3,3,WHITE);
        // OLED.drawLine(38,SCREEN_HALF_HEIGHT_INFO,40,SCREEN_HALF_HEIGHT_INFO+2,WHITE);
        // OLED.drawLine(40,SCREEN_HALF_HEIGHT_INFO+2,40,SCREEN_HALF_HEIGHT_INFO+4,WHITE);
        // OLED.drawLine(40,SCREEN_HALF_HEIGHT_INFO+4,38,SCREEN_HALF_HEIGHT_INFO+6,WHITE);
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

        OLED.display();
    }
};
