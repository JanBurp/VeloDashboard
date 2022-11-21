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

#include "DashboardClass.h"
#include "BatteryClass.h"
#include "OutputClass.h"
#include "IndicatorClass.h"
#include "LightsClass.h"
#include "SpeedClass.h"
#include "LEDClass.h"
#include "LEDstripClass.h"
#include "DisplayClass.h"

// RTC Time for clock
time_t RTCTime;

// Timers
PeriodicTimer speedCalculationTimer;

// Inputs & Outputs
DashboardClass Dashboard;
OutputClass Buzzer;

// Classes
DisplayClass Display;
BatteryClass Battery;
IndicatorClass Indicators;
LightsClass Lights;
SpeedClass Speed;
LEDstripClass LEDstrips;
LedClass LedHeadLightLeft,LedHeadLightRight,LedRearLight;

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
    if (DEBUG)
    {
        Serial.begin(115200);
    }

    // Disable unused pins
    int unusedPins[] = {0, 1, 4, 5, 6, 7, 8, 9, 13, 14,15, 16,17, 20,21 };
    for (size_t pin = 0; pin < 10; pin++)
    {
        pinMode(unusedPins[pin], INPUT_DISABLE);
    }

    // Clock
    setSyncProvider(getTeensy3Time);

    // Knobs & Buttons
    Dashboard.init(PIN_DASHBOARD);
    Battery.init(PIN_BATTERY_METER,PIN_POWER_OFF);
    Battery.loop();

    LedHeadLightLeft.init(PIN_HEAD_LIGHT_LEFT);
    LedRearLight.init(PIN_REAR_LIGHT);
    Lights.init(&Battery,&LedHeadLightLeft,&LedRearLight);

    // Buzzer
    pinMode(PIN_BUZZER, OUTPUT);
    buzzer(false);

    Speed.init();
    Indicators.init();
    LEDstrips.init(&Indicators,&Battery);


    Display.init(&Speed, &Battery, &Indicators, &Lights, &LEDstrips);
    Display.setDisplayMode(DISPLAY_WELCOME);
    Display.show();

    if ( !Battery.isDead() ) {
        LEDstrips.startup_animation();
        Lights.increaseBackLights();
    }

    Display.setDisplayMode(DISPLAY_SPEED_AND_TIME);
    Display.show();

    pinMode(PIN_SPEED, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_SPEED), sensorChange, CHANGE);

    speedCalculationTimer.begin([]{ Speed.loop(); },SPEED_CALCULATION_TIMER);
}

/*
  Read Dashboard buttons
*/
void readButtons()
{

    // Break && Horn
    Lights.setBrake( Dashboard.isBrake() );
    Lights.setHorn( Dashboard.isHorn() );

    // Others
    if ( Dashboard.read() ) {

        if (Dashboard.isIndicatorLeft() ) {
            Indicators.setLeft();
        }
        if (Dashboard.isIndicatorRight()) {
            Indicators.setRight();
        }

        // Alarm
        if ( Dashboard.isAlarm() ) {
            Indicators.setAlarm();
        }

        // Lights
        if ( Dashboard.isLightsMore() )  {
            Lights.increaseLights();
        }
        if ( Dashboard.isLightsLess() ) {
            Lights.decreaseLights();
        }
        if ( Dashboard.isBackLightsMore() )  {
            Lights.increaseBackLights();
        }
        if ( Dashboard.isBackLightsLess() ) {
            Lights.decreaseBackLights();
        }

        // Display pages
        if (Dashboard.isDisplay()) {
            Display.nextDisplayMode();
        }

    }

}

/*
  Set buzzer on or off
*/
void updateBuzzer()
{
    if (Indicators.isActive())
    {
        buzzer(Indicators.getStateLeft() || Indicators.getStateRight());
    }
    else
    {
        buzzer(false);
    }
}

/*

  ==== LOOP ====

*/
void loop()
{
    readButtons();
    updateBuzzer();
    Battery.loop();

    if (Battery.isDead()) {
        LEDstrips.off();
        Lights.off();
        Display.off();
        if (Battery.delayedPowerOff()) {
            Speed.storeODO();
            Battery.powerOff();
        }
    }
    else {
        LEDstrips.loop();
        Display.show();
    }

}
