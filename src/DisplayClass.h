#pragma once

#include "Arduino.h"
#include "settings.h"
#include "SpeedClass.h"
#include "IndicatorClass.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

class DisplayClass
{

private:
  int displayMode = DISPLAY_SPEED_AND_TIME;
  int firstMode = DISPLAY_SPEED_AND_TIME;
  int lastMode = DISPLAY_TIME;
  SpeedClass *Speedometer;
  IndicatorClass *Indicators;

public:
  void init(SpeedClass *speed, IndicatorClass *indicators)
  {
    this->Speedometer = speed;
    this->Indicators = indicators;
    display.clearDisplay();
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
    this->displayMode = this->firstMode;
  }

  unsigned int _displayMargin(unsigned int textLen)
  {
    return (SCREEN_WIDTH - textLen * 12) / 2;
  }

  void show()
  {
    display.clearDisplay();

    if (this->displayMode == DISPLAY_WELCOME)
    {
      display.setTextSize(4);
      display.setTextColor(WHITE);
      display.setCursor(2, 2);
      display.print("Quest");
      display.setTextSize(3);
      display.setTextColor(WHITE);
      display.setCursor(8, 40);
      display.print("- 631- ");
    }

    else
    {
      // Speed
      float speed = this->Speedometer->getSpeed();
      int decis = (int)speed;
      int precision = (speed - decis) * 10;

      display.setTextSize(5);
      display.setTextColor(WHITE);
      display.setCursor(8, 0);

      char speedStr[3];
      snprintf(speedStr, 3, "%2i", decis);
      display.setCursor(0, 0);
      display.print(speedStr);
      snprintf(speedStr, 3, "%1i", precision);
      display.setCursor(74, 0);
      display.print(speedStr);

      // Sensor
      display.setTextSize(3);
      display.setCursor(54, 14);
      if (this->Speedometer->getSpeedSensor())
      {
        display.print(".");
      }
      else
      {
        display.print(" ");
      }

      // Faster / Slower than average
      if (!this->Speedometer->isPaused())
      {
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(116, 10);
        if (this->Speedometer->isFaster())
          display.print("+");
        else
          display.print("-");
      }

      // Indicators
      if (this->Indicators->getStateLeft())
      {
        display.fillTriangle(0, SCREEN_HALF_HEIGHT, SCREEN_HALF_WIDTH - 2, 0, SCREEN_HALF_WIDTH - 2, SCREEN_HEIGHT, WHITE);
      }
      if (this->Indicators->getStateRight())
      {
        display.fillTriangle(SCREEN_HALF_WIDTH + 2, 0, SCREEN_WIDTH, SCREEN_HALF_HEIGHT, SCREEN_HALF_WIDTH + 2, SCREEN_HEIGHT, WHITE);
      }

      // Data
      switch (this->displayMode)
      {

      case DISPLAY_SPEED_AND_TIME:
        char timeStr[7];
        if ((millis() / 500) % 2 == 0)
        {
          snprintf(timeStr, 7, "%2i:%02i", hour(), minute());
        }
        else
        {
          snprintf(timeStr, 7, "%2i %02i", hour(), minute());
        }
        display.setTextSize(3);
        display.setCursor(this->_displayMargin(7), SCREEN_HALF_HEIGHT_VALUES - 6);
        display.print(timeStr);
        break;

      case DISPLAY_SPEEDS:
        char avgSpeedStr[6];
        snprintf(avgSpeedStr, 6, "%-4.1f", this->Speedometer->getAvgSpeed());
        char maxSpeedStr[6];
        snprintf(maxSpeedStr, 6, "%4.1f", this->Speedometer->getMaxSpeed());

        display.setTextSize(1);
        display.setCursor(0, SCREEN_HALF_HEIGHT_INFO);
        display.print("avg");
        display.setCursor(SCREEN_WIDTH - 20, SCREEN_HALF_HEIGHT_INFO);
        display.print("max");

        display.setTextSize(2);
        display.setCursor(0, SCREEN_HALF_HEIGHT_VALUES);
        display.print(avgSpeedStr);
        display.setCursor(80, SCREEN_HALF_HEIGHT_VALUES);
        display.print(maxSpeedStr);
        break;

      case DISPLAY_DISTANCE:
        char distStr[8];
        snprintf(distStr, 8, "%-6.2f", this->Speedometer->getDistance());

        display.setTextSize(1);
        display.setCursor(0, SCREEN_HALF_HEIGHT_INFO);
        display.print("dist");
        display.setCursor(SCREEN_WIDTH - 20, SCREEN_HALF_HEIGHT_INFO);
        display.print("odo");

        display.setTextSize(2);
        display.setCursor(0, SCREEN_HALF_HEIGHT_VALUES);
        display.print(distStr);

        snprintf(distStr, 8, "%5lu", this->Speedometer->getOdoDistance());
        display.setCursor(68, SCREEN_HALF_HEIGHT_VALUES);
        display.print(distStr);

        break;

      case DISPLAY_TIME:
        char timeTripStr[7];
        unsigned long tripTimeMs = this->Speedometer->getTripTime();
        unsigned long tripTimeSec = tripTimeMs / 1000;
        unsigned int minutes = tripTimeSec / 60;
        unsigned int seconds = tripTimeSec % 60;
        snprintf(timeTripStr, 7, "%2u:%02u", minutes, seconds);

        // char usedMilliAmps[5];
        // unsigned int milliAmps = LEDstrips.max_used_milliamps();
        // snprintf(usedMilliAmps, 5, "%4u", milliAmps );

        display.setTextSize(1);
        display.setCursor(0, SCREEN_HALF_HEIGHT_INFO);
        display.print("trip time");

        // display.setCursor(SCREEN_WIDTH - 16, SCREEN_HALF_HEIGHT_INFO);
        // display.print("mA");

        display.setTextSize(2);
        display.setCursor(0, SCREEN_HALF_HEIGHT_VALUES);
        display.print(timeTripStr);
        // display.setCursor(80, SCREEN_HALF_HEIGHT_VALUES);
        // display.print(usedMilliAmps);

        break;
      }
    }

    display.display();
  }
};
