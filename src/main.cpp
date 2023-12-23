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
#include "IdleClass.h"
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
PeriodicTimer speedCalculationTimer(TCK);

// Inputs & Outputs
DashboardClass Dashboard;
OutputClass Buzzer;

// Classes
DisplayClass Display;
BatteryClass Battery;
IdleClass IdleTimer;
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
    IdleTimer.action();
}

/**
 * ==== SETUP ====
 */
void setup()
{
    if (DEBUG)
    {
        Serial.begin(115200);
        Serial.println("\n");
        Serial.print("F_CPU_ACTUAL = (Mhz)\t");
        Serial.println(F_CPU_ACTUAL / 1000000);
    }

    pinMode(PIN_LED_BRAKE, OUTPUT);

    // Disable unused pins (saves a bit current)
    pinMode(INTERNAL_LED,OUTPUT);
    digitalWrite(INTERNAL_LED,LOW);
    int unusedPins[] = UNUSED_PINS;
    for (size_t pin = 0; pin < NR_UNUSED_PINS; pin++)
    {
        pinMode(unusedPins[pin], INPUT_DISABLE);
    }

    // Clock
    setSyncProvider(getTeensy3Time);

    // Knobs & Buttons
    Dashboard.init(PIN_BRAKE,PIN_BUTTONS_LEFT,PIN_BUTTONS_RIGHT);
    Battery.init(PIN_BATTERY_METER,PIN_POWER_OFF);
    Battery.loop();
    IdleTimer.action();

    Speed.init();

    LedHeadLightLeft.init(PIN_HEAD_LIGHT_LEFT);
    LedHeadLightRight.init(PIN_HEAD_LIGHT_RIGHT);
    LedRearLight.init(PIN_REAR_LIGHT);
    LedBrakeLight.init(PIN_BRAKE_LIGHT);
    Lights.init(&Battery, &Speed, &LedHeadLightLeft,&LedHeadLightRight,&LedRearLight,&LedBrakeLight);

    Indicators.init(PIN_LED_LEFT,PIN_LED_RIGHT);
    LEDstrips.init(&Indicators,&Lights,&Battery,&IdleTimer,&Speed);

    // Buzzer
    pinMode(PIN_BUZZER, OUTPUT);
    buzzer(false);



    Display.init(&Speed, &Battery, &IdleTimer, &Indicators, &Lights, &LEDstrips);
    Display.setDisplayMode(DISPLAY_WELCOME);
    Display.setContrast(1);
    Display.show();

    if ( !Battery.isDead() ) {
        LEDstrips.startup_animation();
    }

    if ( Speed.IsNewDay() ) {
        Speed.startDay();
    }
    else {
        if ( Speed.IsShortPause() ) {
            Display.askStartupQuestion();
            Display.show();
            Dashboard.waitForButtonPress();
            if ( Dashboard.isLightsUp() ) {
                Speed.continueCurrent();
            }
            else {
                Speed.resetCurrent();
            }
        }
        else {
            Speed.resetCurrent();
        }
    }

    Display.setDisplayMode(DISPLAY_HOME);
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

    // Break & Beam
    Lights.setBrake( Dashboard.isBrake() );
    digitalWrite(PIN_LED_BRAKE, Dashboard.isBrake());
    Lights.setBeam( Dashboard.isBeam() );

    // Others
    if ( Dashboard.read() ) {
        IdleTimer.action();

        // LEFT - RIGHT
        if ( Display.isSettingsMenu() ) {
            if ( Dashboard.isIndicatorLeft() ) {
                Display.moveCursor( 1 );
            }
            if ( Dashboard.isIndicatorRight() ) {
                Display.moveCursor( -1 );
            }
        }
        else {
            if ( Dashboard.isIndicatorLeft() ) {
                Indicators.setLeft();
            }
            if ( Dashboard.isIndicatorRight() ) {
                Indicators.setRight();
            }
        }

        // UP - DOWN
        if ( Display.isResetTripMenu() ) {
            if ( Dashboard.isLightsUp() )  {
                Speed.resetTripDistance(1);
            }
            if ( Dashboard.isLightsDown() )  {
                Speed.resetTripDistance(2);
            }
            if ( Dashboard.isIndicatorLeft() )  {
                Speed.resetTripDistance(3);
            }
        }
        else if ( Display.isSetClockMenu() ) {
            if ( Dashboard.isLightsUp() )  {
                Speed.increaseClock( Display.cursorAmount() );
            }
            if ( Dashboard.isLightsDown() )  {
                Speed.decreaseClock( Display.cursorAmount() );
            }
        }
        else if ( Display.isSetTyreMenu() ) {
            if ( Dashboard.isLightsUp() )  {
                Speed.increaseCircumference( Display.cursorAmount() );
            }
            if ( Dashboard.isLightsDown() )  {
                Speed.decreaseCircumference( Display.cursorAmount() );
            }
        }
        else if ( Display.isSetTotalMenu() ) {
            if ( Dashboard.isLightsUp() )  {
                Speed.increaseTotal( Display.cursorAmount() );
            }
            if ( Dashboard.isLightsDown() )  {
                Speed.decreaseTotal( Display.cursorAmount() );
            }
        }
        else {
            if ( Dashboard.isLightsUp() )  {
                Display.setDisplayModeHome();
                Lights.increaseLights();
                if ( Dashboard.isLongPressed() ) {
                    Lights.setFogLight();
                }
            }
            if ( Dashboard.isLightsDown() )  {
                Display.setDisplayModeHome();
                Lights.decreaseLights();
                if ( Dashboard.isLongPressed() ) {
                    Lights.resetLights();
                }
            }
        }

        // Display pages
        if (Dashboard.isDisplay()) {
            Display.nextDisplayMode();
        }
        if ( Dashboard.isDisplay() && Dashboard.isLongPressed() ) {
            Display.toggleSettings();
        }

    }

    if ( IdleTimer.warning() ) {
        Display.setDisplayModeHome();
        Lights.resetLights();
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

    if ( IdleTimer.ended() ) {
        Speed.storeMemory();
        LEDstrips.off();
        Lights.off();
        Display.off();
        Battery.powerOff();
    }

    if (Battery.isDead()) {
        LEDstrips.off();
        Lights.off();
        Display.off();
        if (Battery.delayedPowerOff()) {
            Speed.storeMemory();
            Battery.powerOff();
        }
    }
    else {
        LEDstrips.loop();
        Lights.loop();
        Display.show();
    }

    if ( !Dashboard.isBrake() ) {
        Battery.loop();
    }

}
