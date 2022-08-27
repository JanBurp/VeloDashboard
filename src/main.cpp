/**
 *
 * VeloDashboard
 *
 *  (c) Jan den Besten
 *
 */

#define DEBUG true

#include "Button.cpp"
#include "Output.cpp"
#include "LEDstrips.cpp"
#include "Speed.cpp"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);



/**
 * other PINS
 */

#define INPUT_INDICATOR_RIGHT     2
#define INPUT_INDICATOR_LEFT      3
#define INPUT_ALARM               4

#define OUTPUT_LED_RIGHT          5
#define OUTPUT_LED_LEFT           6
#define OUTPUT_BUZZER             7 // PWM

#define SPEED_INPUT               14 // A0 // D14

/**
 * DEFAULTS
 */

#define BUZZER_TONE               660
#define INDICATOR_TIME            800
#define ALARM_TIME                400

/**
 * INPUTS & OUTPUTS
 */

Button ButtonIndicatorRight;
Button ButtonIndicatorLeft;
Button ButtonIndicatorAlarm;
Output LedRight;
Output LedLeft;
Output Buzzer;
Speed SpeedoMeter;
LEDstrips LEDstrips;

enum displayType
{
    WELCOME,
    SPEED,
    DISTANCE,
    TIME,
};

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


void displayShow( displayType type ) {
    display.clearDisplay();
    switch (type) {

        case WELCOME:
            display.setTextSize(4);
            display.setTextColor(WHITE);
            display.setCursor(2, 2);
            display.println("Quest");
            display.setCursor(20, 80);
            display.setTextSize(3);
            display.println("631");
            break;

        case SPEED:
            display.setTextSize(5);
            display.setTextColor(WHITE);
            display.setCursor(2, 2);
            display.println(SpeedoMeter.getSpeedasString());
            display.setTextSize(2);
            display.setCursor(2, 50);
            display.println(SpeedoMeter.getAvgSpeedasString());
            display.setCursor(SCREEN_WIDTH / 2, 50);
            display.println(SpeedoMeter.getMaxSpeedasString());
            break;

        default:
            break;
    }
    display.display();
}

/**
 *
 * ==== SETUP ====
 *
 */

void setup()
{
    if (DEBUG) Serial.begin(9600);

    // Indicator inputs
    ButtonIndicatorRight.init(INPUT_INDICATOR_RIGHT);
    ButtonIndicatorLeft.init(INPUT_INDICATOR_LEFT);
    ButtonIndicatorAlarm.init(INPUT_ALARM);
    // Indicator LEDS
    LedRight.init(OUTPUT_LED_RIGHT);
    LedLeft.init(OUTPUT_LED_LEFT);

    pinMode(OUTPUT_BUZZER, OUTPUT);
    buzzer(false);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    { // Address 0x3D for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }

    displayShow(WELCOME);

    SpeedoMeter.init(SPEED_INPUT);

    LEDstrips.startup_animation();
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

    // Loops
    LedRight.loop();
    LedLeft.loop();
    SpeedoMeter.loop();
    LEDstrips.loop();

    if ( SpeedoMeter.isUpdated() ) {
        displayShow(SPEED);
    }


    // if (DEBUG) {
    //     Serial.print("\tINDICATOR: \t");  Serial.print(IndicatorState);
    //     Serial.print("\tALARM: \t");      Serial.print(AlarmState);
    //     Serial.println("");
    // };

}

