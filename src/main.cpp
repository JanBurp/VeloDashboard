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

#include "ButtonClass.h"
#include "OutputClass.h"
#include "IndicatorClass.h"
#include "SpeedClass.h"
#include "LEDstripClass.h"
#include "DisplayClass.h"

// RTC Time for clock
time_t RTCTime;

// Timers
PeriodicTimer calculationTimer;


// Inputs & Outputs
ButtonClass ButtonIndicatorRight, ButtonIndicatorLeft, ButtonIndicatorAlarm, ButtonLights, DisplayButtonLeft, DisplayButtonRight;
OutputClass Buzzer;

// Classes
DisplayClass Display;
IndicatorClass Indicators;
SpeedClass Speed;
LEDstripClass LEDstrips;



/**
 * Get the Teensy3 Time object
 */
time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}


/**
 * buzzer
 */
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
 * Speed sensor trigger
 *
 */
void sensorChange()
{
  Speed.sensorTrigger();
}



/**
 * ==== SETUP ====
 */
void setup()
{
  if (DEBUG) {
    Serial.begin(9600);
  }

  // Disable unused pins
  int unusedPins[] = {0, 1, 4, 5, 6, 7, 8, 9, 10, 13, 16, 17};
  for (size_t pin = 0; pin < 10; pin++)
  {
    pinMode(unusedPins[pin], INPUT_DISABLE);
  }

  // Clock
  setSyncProvider(getTeensy3Time);

  // Knobs & Buttons
  ButtonIndicatorRight.init(PIN_INPUT_INDICATOR_RIGHT);
  ButtonIndicatorLeft.init(PIN_INPUT_INDICATOR_LEFT);
  ButtonIndicatorAlarm.init(PIN_INPUT_ALARM);
  DisplayButtonLeft.init(PIN_INPUT_DISPLAY_SWITCH_LEFT);
  DisplayButtonRight.init(PIN_INPUT_DISPLAY_SWITCH_RIGHT);
  ButtonLights.init(PIN_BUTTON_WHITE);

  // Buzzer
  pinMode(PIN_BUZZER, OUTPUT);
  buzzer(false);

  LEDstrips.startup_animation();

  Speed.init();
  Indicators.init();

  Display.init(&Speed, &Indicators);
  Display.setDisplayMode(DISPLAY_WELCOME);
  Display.show();

  Display.setDisplayMode(DISPLAY_SPEED_AND_TIME);
  Display.show();

  pinMode(PIN_SPEED, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_SPEED), sensorChange, CHANGE);

  calculationTimer.begin( [] { Speed.loop(); }, SPEED_CALCULATION_TIMER );
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
    Display.resetDisplayMode();
  }
  if (DisplayButtonRight.readOnce())
  {
    Display.nextDisplayMode();
  }

  // Animation
  if (ButtonLights.readLongPress(2000))
  {
    LEDstrips.startup_animation();
  }

  // Loops
  LEDstrips.loop(Indicators.isActive(), Indicators.getStateLeft(), Indicators.getStateRight());

  Display.show();

  // Fake Speed for testing
  if (TEST)
  {
    sensorChange();
  }
}
