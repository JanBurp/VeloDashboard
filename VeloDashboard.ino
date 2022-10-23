/**
 *
 * VeloDashboard
 *
 *  (c) Jan den Besten
 *
 */

#define DEBUG true

#include "lib/Button.cpp"
#include "lib/Output.cpp"
#include "lib/LEDstrips.cpp"



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

#define PIN_INPUT_INDICATOR_RIGHT     2
#define PIN_INPUT_INDICATOR_LEFT      3
#define PIN_INPUT_ALARM               4

#define OUTPUT_LED_RIGHT          5
#define OUTPUT_LED_LEFT           6
#define PIN_BUZZER             9 // PWM

#define PIN_SPEED               14 // A0 // D14

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
// LEDstrips LEDstrips;

/**
 * VARIABLES
 */

bool AlarmState = false;
int IndicatorState = 0;


bool SpeedSensor = false;
unsigned long LastSensorTimeMs = 0;
unsigned long startTime = 0;
float maxSpeed = 0;
float avgSpeed = 0;


/**
 *
 * ==== SETUP ====
 *
 */

void setup() {
    if (DEBUG) Serial.begin(9600);

    // Indicator inputs
    ButtonIndicatorRight.init(PIN_INPUT_INDICATOR_RIGHT);
    ButtonIndicatorLeft.init(PIN_INPUT_INDICATOR_LEFT);
    ButtonIndicatorAlarm.init(PIN_INPUT_ALARM);
    // Indicator LEDS
    LedRight.init(OUTPUT_LED_RIGHT);
    LedLeft.init(OUTPUT_LED_LEFT);

    pinMode(PIN_BUZZER, OUTPUT);
    buzzer(false);


    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }

    // LEDstrips.startup_animation();

    pinMode(PIN_SPEED, INPUT_PULLUP);
    LastSensorTimeMs = millis();
    startTime = millis();

}


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
            // LEDstrips.blink(LEFT,INDICATOR_TIME);
            break;
        case RIGHT :
            LedRight.blink(INDICATOR_TIME);
            LedLeft.off();
            // LEDstrips.blink(RIGHT,INDICATOR_TIME);
            break;
        default:
            LedRight.off();
            LedLeft.off();
            // LEDstrips.normal(BOTH);
    }
}

void buzzer(bool state) {
    if (state) {
        tone(PIN_BUZZER, BUZZER_TONE);
    }
    else {
        noTone(PIN_BUZZER);
    }
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
            // LEDstrips.blink(BOTH,ALARM_TIME);
        }
        else {
            LedRight.off();
            LedLeft.off();
            // LEDstrips.normal(BOTH);
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
    // LEDstrips.loop();


    bool sensor = digitalRead(PIN_SPEED);
    if (sensor != SpeedSensor) {
        SpeedSensor = sensor;
        if ( SpeedSensor ) {
            float SensorTime = millis() - LastSensorTimeMs;
            LastSensorTimeMs = millis();

            float RPM = ( 1000.0 / SensorTime ) * 60.0;
            float Speed = (RPM * 1.4 * 60 / 1000); // 1.4 = omtrek

            if ( Speed <1000 && Speed >= maxSpeed ) {
                maxSpeed = Speed;
            }




            display.clearDisplay();
            display.setTextSize(4);
            display.setTextColor(WHITE);
            display.setCursor(5,10);
            display.println( Speed );

            display.setTextSize(2);
            display.setCursor(5,50);
            display.println( maxSpeed );

            display.display();

            Serial.println( Speed );
        }
    }

    // if (DEBUG) {
    //     Serial.print("\tINDICATOR: \t");  Serial.print(IndicatorState);
    //     Serial.print("\tALARM: \t");      Serial.print(AlarmState);
    //     Serial.println("");
    // };

}

