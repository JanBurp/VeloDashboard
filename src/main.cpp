/**
 *
 * VeloDashboard
 *
 *  (c) Jan den Besten
 *
 */

#include "settings.h"

#include <TimeLib.h>
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

#include "Button.h"
#include "Output.h"
#include "IndicatorClass.h"
#include "SpeedClass.h"
#include "LEDstripClass.h"
#include "DisplayClass.h"

time_t RTCTime;
PeriodicTimer calculationTimer;

/**
 * INPUTS & OUTPUTS
 */

Button ButtonIndicatorRight;
Button ButtonIndicatorLeft;
Button ButtonIndicatorAlarm;
Button ButtonLights;
Button DisplayButtonLeft;
Button DisplayButtonRight;
Output Buzzer;

DisplayClass OLED;
IndicatorClass Indicators;
SpeedClass SpeedoMeter;
LEDstripClass LEDstrips;

void buzzer(bool state)
{
  if (state)
  {
    tone(PIN_BUZZER, BUZZER_TONE);
  }
  else
  {
    noTone(PIN_BUZZER);
  }
}

/**
 * @brief Get the Teensy3 Time object
 *
 * @return time_t
 */

time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}

/**
 *
 * ==== SETUP ====
 *
 */
void sensorChange()
{
  SpeedoMeter.sensorTrigger();
}

void setup()
{
  if (DEBUG)
    Serial.begin(9600);

  // Disable unused pins
  int unusedPins[] = {0, 1, 4, 5, 6, 7, 8, 9, 10, 13, 16, 17};
  for (size_t pin = 0; pin < 10; pin++)
  {
    pinMode(unusedPins[pin], INPUT_DISABLE);
  }

  setSyncProvider(getTeensy3Time);

  // Indicator inputs
  ButtonIndicatorRight.init(PIN_INPUT_INDICATOR_RIGHT);
  ButtonIndicatorLeft.init(PIN_INPUT_INDICATOR_LEFT);
  ButtonIndicatorAlarm.init(PIN_INPUT_ALARM);

  DisplayButtonLeft.init(PIN_INPUT_DISPLAY_SWITCH_LEFT);
  DisplayButtonRight.init(PIN_INPUT_DISPLAY_SWITCH_RIGHT);

  ButtonLights.init(PIN_BUTTON_WHITE);

  pinMode(PIN_BUZZER, OUTPUT);
  buzzer(false);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  LEDstrips.startup_animation();

  SpeedoMeter.init();
  Indicators.init();

  OLED.init(&SpeedoMeter, &Indicators);
  OLED.setDisplayMode(DISPLAY_WELCOME);
  OLED.show();

  OLED.setDisplayMode(DISPLAY_SPEED_AND_TIME);
  OLED.show();

  pinMode(PIN_SPEED, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_SPEED), sensorChange, CHANGE);

  calculationTimer.begin( [] { SpeedoMeter.loop(); }, SPEED_CALCULATION_TIMER );
}

/**
 *
 * ==== MAIN LOOP - Reading buttons ====
 *
 */

void loop()
{

  // ALARM
  if (ButtonIndicatorAlarm.readOnce())
  {
    Indicators.toggleAlarm();
  }

  // INDICATORS (only when there is no alarm)
  if (!Indicators.isAlarmSet())
  {
    if (ButtonIndicatorRight.read())
    {
      Indicators.setRight();
    }
    else if (ButtonIndicatorLeft.read())
    {
      Indicators.setLeft();
    }
    else
    {
      Indicators.reset();
    }
  }

  // BUZZER
  if (Indicators.isActive())
  {
    buzzer(Indicators.getStateLeft() || Indicators.getStateRight());
  }
  else
  {
    buzzer(false);
  }

  // Display page
  if (DisplayButtonLeft.readOnce())
  {
    OLED.resetDisplayMode();
  }
  if (DisplayButtonRight.readOnce())
  {
    OLED.nextDisplayMode();
  }

  // Animation
  if (ButtonLights.readLongPress(2000))
  {
    LEDstrips.startup_animation();
  }

  // Loops
  LEDstrips.loop(Indicators.isActive(), Indicators.getStateLeft(), Indicators.getStateRight());

  OLED.show();

  // Fake Speed for testing
  if (TEST)
  {
    sensorChange();
  }
}
