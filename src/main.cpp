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
LedClass LedHeadLightLeft,LedHeadLightRight,LedRearLight,LedBrakeLight;

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

    // Disable unused pins (saves a bit current)
    int unusedPins[] = UNUSED_PINS;
    for (size_t pin = 0; pin < NR_UNUSED_PINS; pin++)
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
    LedHeadLightRight.init(PIN_HEAD_LIGHT_RIGHT);
    LedRearLight.init(PIN_REAR_LIGHT);
    LedBrakeLight.init(PIN_BRAKE_LIGHT);
    Lights.init(&Battery,&LedHeadLightLeft,&LedHeadLightRight,&LedRearLight,&LedBrakeLight);

    // Buzzer
    pinMode(PIN_BUZZER, OUTPUT);
    buzzer(false);

    // Horn
    pinMode(PIN_HORN, OUTPUT);
    analogWrite(PIN_HORN, 0);

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
        if ( Dashboard.isLights() )  {
            if ( Dashboard.isLongPress() ) {
                Lights.resetLights();
            }
            else {
                Lights.increaseLights();
            }
        }
        // if ( Dashboard.isBackLights() )  {
        //     if ( Dashboard.isLongPress() ) {
        //         Lights.resetLights();
        //     }
        //     else {
        //         Lights.increaseBackLights();
        //     }
        // }

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
  HORN on or off
*/
void updateHorn()
{
    if (Lights.getHorn())
    {
        analogWrite(PIN_HORN, HORN_LOUDNESS);
    }
    else
    {
        analogWrite(PIN_HORN, 0);
    }

}


/*

  ==== LOOP ====

*/
void loop()
{
    readButtons();
    updateBuzzer();
    updateHorn();
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
