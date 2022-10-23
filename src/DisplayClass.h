#pragma once

#include "Arduino.h"
#include "settings.h"
#include "SpeedClass.h"
#include "IndicatorClass.h"

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

public:
  void init(SpeedClass *speed, IndicatorClass *indicators)
  {
    this->Speed = speed;
    this->Indicators = indicators;
    // Address 0x3D for 128x64
    if (!OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      Serial.println(F("SSD1306 allocation failed"));
      for (;;) {
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
    this->displayMode = this->firstMode;
  }


  void _show_welcome() {
    OLED.setTextSize(4);
    OLED.setCursor(2, 2);
    OLED.print("Quest");
    OLED.setTextSize(3);
    OLED.setCursor(8, 40);
    OLED.print("- 631- ");
  }

  void _show_speed() {
    float speed = this->Speed->getSpeed();
    int decis = (int)speed;
    int precision = (speed - decis) * 10;

    OLED.setTextSize(5);

    char speedStr[3];
    snprintf(speedStr, 3, "%2i", decis);
    OLED.setCursor(40, 0);
    OLED.print(speedStr);
    snprintf(speedStr, 3, "%1i", precision);
    OLED.setTextSize(3);
    OLED.setCursor(110, 14);
    OLED.print(speedStr);

    // Sensor
    OLED.setTextSize(3);
    OLED.setCursor(94, 14);
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
      OLED.setTextSize(2);
      OLED.setCursor(0, 10);
      if (this->Speed->isFaster()) {
        OLED.print("+");
      }
      else {
        OLED.print("-");
      }
    }
  }

  void _show_indicators() {
    if (this->Indicators->getStateLeft())
    {
      OLED.fillTriangle(0, SCREEN_HALF_HEIGHT, SCREEN_HALF_WIDTH - 2, 0, SCREEN_HALF_WIDTH - 2, SCREEN_HEIGHT, WHITE);
    }
    if (this->Indicators->getStateRight())
    {
      OLED.fillTriangle(SCREEN_HALF_WIDTH + 2, 0, SCREEN_WIDTH, SCREEN_HALF_HEIGHT, SCREEN_HALF_WIDTH + 2, SCREEN_HEIGHT, WHITE);
    }
  }

  void _show_time() {
    char timeStr[7];
    if ((millis() / 500) % 2 == 0)
    {
      snprintf(timeStr, 7, "%2i:%02i", hour(), minute());
    }
    else
    {
      snprintf(timeStr, 7, "%2i %02i", hour(), minute());
    }
    OLED.setTextSize(3);
    OLED.setCursor(36, SCREEN_HALF_HEIGHT_VALUES - 6);
    OLED.print(timeStr);
  }

  void _show_speeds() {
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

  void _show_distances() {
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

  void _show_triptime() {
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
          this->_show_time();
          break;
        case DISPLAY_SPEEDS:
          this->_show_speeds();
          break;
        case DISPLAY_DISTANCE:
          this->_show_distances();
          break;
        case DISPLAY_TRIPTIME:
          this->_show_triptime();
          break;
        }
    }

    OLED.display();
  }
};
