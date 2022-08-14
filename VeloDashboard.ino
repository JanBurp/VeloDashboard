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
// #include "lib/Input.cpp"


/**
 * PINS
 */

#define INPUT_INDICATOR_RIGHT     2
#define INPUT_INDICATOR_LEFT      3
#define INPUT_ALARM               4

#define OUTPUT_LED_RIGHT          5
#define OUTPUT_LED_LEFT           6

Button ButtonIndicatorRight;
Button ButtonIndicatorLeft;
Button ButtonIndicatorAlarm;
Output LedRight;
Output LedLeft;



bool AlarmState = false;
int IndicatorState = 0;

/**
 * 
 * ==== SETUP ====
 * 
 */

void setup() {
    if (DEBUG) Serial.begin(9600);

    // Indicator inputs
    ButtonIndicatorRight.init(INPUT_INDICATOR_RIGHT);
    ButtonIndicatorLeft.init(INPUT_INDICATOR_LEFT);
    ButtonIndicatorAlarm.init(INPUT_ALARM);
    // Indicator LEDS
    LedRight.init(OUTPUT_LED_RIGHT);
    LedLeft.init(OUTPUT_LED_LEFT);
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
        case 1 :
            LedRight.blink();
            LedLeft.off();
            break;
        case -1 :
            LedRight.off();
            LedLeft.blink();
            break;
        default:
            LedRight.off();
            LedLeft.off();
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
            LedRight.blink();
            LedLeft.blink();
        }
        else {
            LedRight.off();
            LedLeft.off();
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



    // Output loops
    LedRight.loop();
    LedLeft.loop();

    if (DEBUG) {
        Serial.print("\tINDICATOR: \t");  Serial.print(IndicatorState);
        Serial.print("\tALARM: \t");      Serial.print(AlarmState);
        Serial.println("");
    };

}
