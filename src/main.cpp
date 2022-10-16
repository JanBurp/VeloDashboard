/**
 *
 * VeloDashboard
 *
 *  (c) Jan den Besten
 *
 */

#define DEBUG false
#define TEST true   // Test setup @ home

#include "Button.cpp"
#include "Output.cpp"
#include "IndicatorClass.h"
#include "LEDstrips.cpp"
#include "Speed.cpp"

#include <TimeLib.h>
time_t RTCTime;

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128        // OLED display width, in pixels
#define SCREEN_HEIGHT 64        // OLED display height, in pixels
#define SCREEN_HALF_HEIGHT 32
#define SCREEN_HALF_WIDTH 64
#define SCREEN_HALF_HEIGHT_INFO   40
#define SCREEN_HALF_HEIGHT_VALUES 50

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/**
 * other PINS
 */

#define UI_BUTTON_WHITE         23
#define UI_BUTTON_RED           20
#define UI_BUTTON_YELLOW_LEFT   22
#define UI_BUTTON_YELLOW_RIGHT  21

#define INPUT_INDICATOR_RIGHT     14
#define INPUT_INDICATOR_LEFT      15
#define INPUT_ALARM               UI_BUTTON_RED

#define OUTPUT_BUZZER             12 // PWM
#define SPEED_INPUT               11

#define DISPLAY_SWITCH_LEFT       UI_BUTTON_YELLOW_LEFT
#define DISPLAY_SWITCH_RIGHT      UI_BUTTON_YELLOW_RIGHT

/**
 * DEFAULTS
 */

#define DISPLAY_WELCOME           0
#define DISPLAY_SPEED_AND_TIME    1
#define DISPLAY_SPEEDS            2
#define DISPLAY_DISTANCE          3
#define DISPLAY_TIME              4

/**
 * TIMERS
 */
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

PeriodicTimer calculationTimer;

#define SPEED_CALCULATION_TIMER  1000ms

#define BUZZER_TONE              660


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

IndicatorClass Indicators;
Speed SpeedoMeter;
LEDstrips LEDstrips;

int CurrentDisplay = DISPLAY_SPEED_AND_TIME;



void buzzer(bool state) {
    if (state) {
        tone(OUTPUT_BUZZER, BUZZER_TONE);
    }
    else {
        noTone(OUTPUT_BUZZER);
    }
}


unsigned int displayMargin( unsigned int textLen ) {
    return (SCREEN_WIDTH - textLen * 12) / 2;
}

void displayShow( int type ) {
    display.clearDisplay();

    if ( type == DISPLAY_WELCOME ) {
        display.setTextSize(4);
        display.setTextColor(WHITE);
        display.setCursor(2, 2);
        display.print("Quest");
        display.setTextSize(3);
        display.setTextColor(WHITE);
        display.setCursor(8, 40);
        display.print("- 631- ");
    }

    else {
        // Speed
        float speed = SpeedoMeter.getSpeed();
        int decis = (int) speed;
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
        if (SpeedoMeter.getSpeedSensor())
        {
            display.print(".");
        }
        else
        {
            display.print(" ");
        }

        // Faster / Slower than average
        if (!SpeedoMeter.isPaused())
        {
            display.setTextSize(2);
            display.setTextColor(WHITE);
            display.setCursor(116, 10);
            if (SpeedoMeter.isFaster())
                display.print("+");
            else
                display.print("-");
        }


        // Indicators
        if ( Indicators.getStateLeft() ) {
            display.fillTriangle(0, SCREEN_HALF_HEIGHT, SCREEN_HALF_WIDTH-2, 0, SCREEN_HALF_WIDTH-2, SCREEN_HEIGHT, WHITE);
        }
        if ( Indicators.getStateRight() ) {
            display.fillTriangle(SCREEN_HALF_WIDTH+2, 0, SCREEN_WIDTH, SCREEN_HALF_HEIGHT, SCREEN_HALF_WIDTH+2, SCREEN_HEIGHT, WHITE);
        }


        // Data
        switch (type)
        {

        case DISPLAY_SPEED_AND_TIME:
            char timeStr[7];
            if ( (millis()/500) %2 == 0 ) {
                snprintf(timeStr, 7, "%2i:%02i", hour(), minute() );
            }
            else {
                snprintf(timeStr, 7, "%2i %02i", hour(), minute());
            }
            display.setTextSize(3);
            display.setCursor( displayMargin(7) , SCREEN_HALF_HEIGHT_VALUES-6);
            display.print(timeStr);
            break;

        case DISPLAY_SPEEDS:
            char avgSpeedStr[6];
            snprintf(avgSpeedStr, 6, "%-4.1f", SpeedoMeter.getAvgSpeed());
            char maxSpeedStr[6];
            snprintf(maxSpeedStr, 6, "%4.1f", SpeedoMeter.getMaxSpeed());

            display.setTextSize(1);
            display.setCursor(0, SCREEN_HALF_HEIGHT_INFO);
            display.print("avg");
            display.setCursor(SCREEN_WIDTH-20, SCREEN_HALF_HEIGHT_INFO);
            display.print("max");

            display.setTextSize(2);
            display.setCursor(0, SCREEN_HALF_HEIGHT_VALUES);
            display.print(avgSpeedStr);
            display.setCursor(80, SCREEN_HALF_HEIGHT_VALUES);
            display.print(maxSpeedStr);
            break;

        case DISPLAY_DISTANCE:
            char distStr[8];
            snprintf(distStr, 8, "%-6.2f", SpeedoMeter.getDistance());

            display.setTextSize(1);
            display.setCursor(0, SCREEN_HALF_HEIGHT_INFO);
            display.print("dist");
            display.setCursor(SCREEN_WIDTH - 20, SCREEN_HALF_HEIGHT_INFO);
            display.print("odo");

            display.setTextSize(2);
            display.setCursor(0, SCREEN_HALF_HEIGHT_VALUES);
            display.print(distStr);

            snprintf(distStr, 8, "%5lu", SpeedoMeter.getOdoDistance());
            display.setCursor(68, SCREEN_HALF_HEIGHT_VALUES);
            display.print(distStr);

            break;

        case DISPLAY_TIME:
            char timeTripStr[7];
            unsigned long tripTimeMs = SpeedoMeter.getTripTime();
            unsigned long tripTimeSec = tripTimeMs / 1000;
            unsigned int minutes = tripTimeSec / 60;
            unsigned int seconds = tripTimeSec % 60;
            snprintf(timeTripStr, 7, "%2u:%02u", minutes,seconds );

            char usedMilliAmps[5];
            unsigned int milliAmps = LEDstrips.max_used_milliamps();
            snprintf(usedMilliAmps, 5, "%4u", milliAmps );

            display.setTextSize(1);
            display.setCursor(0, SCREEN_HALF_HEIGHT_INFO);
            display.print("trip time");

            display.setCursor(SCREEN_WIDTH - 16, SCREEN_HALF_HEIGHT_INFO);
            display.print("mA");

            display.setTextSize(2);
            display.setCursor(0, SCREEN_HALF_HEIGHT_VALUES);
            display.print(timeTripStr);
            display.setCursor(80, SCREEN_HALF_HEIGHT_VALUES);
            display.print(usedMilliAmps);

            break;
        }

    }

    display.display();
}


/**
 * @brief Get the Teensy3 Time object
 *
 * @return time_t
 */

time_t getTeensy3Time() {
    return Teensy3Clock.get();
}


/**
 *
 * ==== SETUP ====
 *
 */
void sensorChange() {
    SpeedoMeter.sensorTrigger();
}

void setup() {
    if (DEBUG) Serial.begin(9600);

    // Disable unused pins
    int unusedPins[] = {0,1,4,5,6,7,8,9,10,13,16,17};
    for (size_t pin = 0; pin < 10; pin++) {
        pinMode(unusedPins[pin],INPUT_DISABLE);
    }

    setSyncProvider( getTeensy3Time );

    // Indicator inputs
    ButtonIndicatorRight.init(INPUT_INDICATOR_RIGHT);
    ButtonIndicatorLeft.init(INPUT_INDICATOR_LEFT);
    ButtonIndicatorAlarm.init(INPUT_ALARM);

    DisplayButtonLeft.init(DISPLAY_SWITCH_LEFT);
    DisplayButtonRight.init(DISPLAY_SWITCH_RIGHT);

    ButtonLights.init(UI_BUTTON_WHITE);

    pinMode(OUTPUT_BUZZER, OUTPUT);
    buzzer(false);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    { // Address 0x3D for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }

    displayShow(DISPLAY_WELCOME);

    SpeedoMeter.init();
    Indicators.init();
    LEDstrips.startup_animation();

    displayShow(CurrentDisplay);

    pinMode(SPEED_INPUT, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(SPEED_INPUT), sensorChange, CHANGE);

    calculationTimer.begin( [] { SpeedoMeter.loop(); }, SPEED_CALCULATION_TIMER );
}

/**
 *
 * ==== MAIN LOOP - Reading buttons ====
 *
 */

void loop() {

    // ALARM
    if ( ButtonIndicatorAlarm.readOnce() ) {
        Indicators.toggleAlarm();
    }

    // INDICATORS (only when there is no alarm)
    if ( ! Indicators.isAlarmSet() ) {
        if ( ButtonIndicatorRight.read() ) {
            Indicators.setRight();
        }
        else if ( ButtonIndicatorLeft.read() ) {
            Indicators.setLeft();
        }
        else {
            Indicators.reset();
        }
    }

    // BUZZER
    if ( Indicators.isActive() ) {
        buzzer( Indicators.getStateLeft() || Indicators.getStateRight() );
    }
    else {
        buzzer(false);
    }

    // Display page
    if ( DisplayButtonLeft.readOnce() ) {
        CurrentDisplay = DISPLAY_SPEED_AND_TIME;
    }
    if (DisplayButtonRight.readOnce()) {
        CurrentDisplay++;
        if (CurrentDisplay > DISPLAY_TIME) {
            CurrentDisplay = DISPLAY_SPEED_AND_TIME;
        }
    }

    // Animation
    if ( ButtonLights.readLongPress(2000) ) {
        LEDstrips.startup_animation();
    }

    // Loops
    LEDstrips.loop( Indicators.isActive(), Indicators.getStateLeft(), Indicators.getStateRight() );

    displayShow(CurrentDisplay);




    // Fake Speed for testing
    if (TEST) {
        sensorChange();
    }
}

