/**
 *
 * VeloDashboard
 *
 *  (c) Jan den Besten
 *
 */

#define DEBUG false

#include "Button.cpp"
#include "Output.cpp"
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

#define OUTPUT_LED_RIGHT          7
#define OUTPUT_LED_LEFT           8
#define OUTPUT_BUZZER             12 // PWM

#define SPEED_INPUT               11

#define DISPLAY_SWITCH_LEFT       UI_BUTTON_YELLOW_LEFT
#define DISPLAY_SWITCH_RIGHT      UI_BUTTON_YELLOW_RIGHT

/**
 * DEFAULTS
 */

#define BUZZER_TONE               660
#define INDICATOR_TIME            800
#define ALARM_TIME                400

#define DISPLAY_WELCOME           0
#define DISPLAY_SPEED_AND_TIME    1
#define DISPLAY_SPEEDS            2
#define DISPLAY_DISTANCE          3
#define DISPLAY_TIME              4


/**
 * INPUTS & OUTPUTS
 */

Button ButtonIndicatorRight;
Button ButtonIndicatorLeft;
Button ButtonIndicatorAlarm;
Button ButtonLights;
Button DisplayButtonLeft;
Button DisplayButtonRight;
Output LedRight;
Output LedLeft;
Output Buzzer;
Speed SpeedoMeter;
LEDstrips LEDstrips;

int CurrentDisplay = DISPLAY_SPEED_AND_TIME;

/**
 * VARIABLES
 */

bool AlarmState = false;
int IndicatorState = 0;


int read_indicators() {
    int IndicatorState = 0;
    if ( ButtonIndicatorRight.read() ) {
        IndicatorState = 1;
    }
    if ( ButtonIndicatorLeft.read() ) {
        IndicatorState = -1;
    }
    return IndicatorState;
}

void change_indicators(int state) {
    switch (state) {
        case LEFT :
            LedRight.off();
            LedLeft.blink(INDICATOR_TIME);
            LEDstrips.blink(LEFT,INDICATOR_TIME);
            break;
        case RIGHT :
            LedRight.blink(INDICATOR_TIME);
            LedLeft.off();
            LEDstrips.blink(RIGHT,INDICATOR_TIME);
            break;
        default:
            LedRight.off();
            LedLeft.off();
            LEDstrips.normal(BOTH);
    }
}

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
        display.setCursor(80, 0);
        display.print(speedStr);

        // Sensor
        display.setTextSize(4);
        display.setCursor(56, 8);
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
            display.setCursor(110, 10);
            if (SpeedoMeter.isFaster())
                display.print("+");
            else
                display.print("-");
        }


        // Indicators
        if ( LedLeft.getState() )
        {
            display.fillTriangle(SCREEN_HALF_WIDTH, 0, SCREEN_WIDTH, SCREEN_HALF_HEIGHT, SCREEN_HALF_WIDTH, SCREEN_HEIGHT, WHITE);
        }
        if ( LedRight.getState() ) {
            display.fillTriangle(0, SCREEN_HALF_HEIGHT, SCREEN_HALF_WIDTH, 0, SCREEN_HALF_WIDTH, SCREEN_HEIGHT, WHITE);
        }


        // Data
        switch (type)
        {

        case DISPLAY_SPEED_AND_TIME:
            char timeStr[7];
            if ( second()%2 ==0 ) {
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
            // char totalTripTimeStr[7];
            // unsigned long totalTimeMs = SpeedoMeter.getTotalTime();
            // unsigned long totalTimeSec = totalTimeMs / 1000;
            // minutes = totalTimeSec / 60;
            // seconds = totalTimeSec % 60;
            // snprintf(totalTripTimeStr, 7, "%2u:%02u", minutes, seconds);

            display.setTextSize(1);
            display.setCursor(0, SCREEN_HALF_HEIGHT_INFO);
            display.print("trip time");
            // display.setCursor(SCREEN_WIDTH - 32, SCREEN_HALF_HEIGHT_INFO);
            // display.print("total");

            display.setTextSize(2);
            display.setCursor(0, SCREEN_HALF_HEIGHT_VALUES);
            display.print(timeTripStr);
            // display.setCursor(68, SCREEN_HALF_HEIGHT_VALUES);
            // display.print(totalTripTimeStr);

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

time_t getTeensy3Time()
{
    return Teensy3Clock.get();
}


/**
 *
 * ==== SETUP ====
 *
 */

void setup()
{
    if (DEBUG) Serial.begin(9600);

    setSyncProvider( getTeensy3Time );

    // Indicator inputs
    ButtonIndicatorRight.init(INPUT_INDICATOR_RIGHT);
    ButtonIndicatorLeft.init(INPUT_INDICATOR_LEFT);
    ButtonIndicatorAlarm.init(INPUT_ALARM);
    // Indicator LEDS
    LedRight.init(OUTPUT_LED_RIGHT);
    LedLeft.init(OUTPUT_LED_LEFT);

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

    SpeedoMeter.init(SPEED_INPUT);
    LEDstrips.startup_animation();

    displayShow(CurrentDisplay);
}

/**
 *
 * ==== MAIN LOOP ====
 *
 */

void loop() {

    // ALARM
    if ( ButtonIndicatorAlarm.readOnce() ) {
        AlarmState = ! AlarmState;
        if ( AlarmState ) {
            IndicatorState = 0; // Indicators to default state
            LedRight.blink(ALARM_TIME);
            LedLeft.blink(ALARM_TIME);
            LEDstrips.blink(BOTH,ALARM_TIME);
        }
        else {
            LedRight.off();
            LedLeft.off();
            LEDstrips.normal(BOTH);
        }
    }

    // INDICATORS (only when there is no alarm)
    if ( ! AlarmState ) {
        int newIndicatorState = read_indicators();
        if ( newIndicatorState != IndicatorState  ) {
            IndicatorState = newIndicatorState;
            change_indicators(IndicatorState);
        }
    }

    // BUZZER
    buzzer( LedRight.getState() || LedLeft.getState() );

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
    LedRight.loop();
    LedLeft.loop();
    SpeedoMeter.loop();
    LEDstrips.loop();

    displayShow(CurrentDisplay);

    // if (DEBUG) {
    //     Serial.print("\tINDICATOR: \t");  Serial.print(IndicatorState);
    //     Serial.print("\tALARM: \t");      Serial.print(AlarmState);
    //     Serial.println("");
    // };

}

