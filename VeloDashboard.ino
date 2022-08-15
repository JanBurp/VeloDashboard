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

/**
 * other PINS
 */

#define INPUT_INDICATOR_RIGHT     2
#define INPUT_INDICATOR_LEFT      3
#define INPUT_ALARM               4

#define OUTPUT_LED_RIGHT          5
#define OUTPUT_LED_LEFT           6
#define OUTPUT_BUZZER             9 // PWM

/**
 * DEFAULTS
 */

#define BUZZER_TIME               660
#define INDICATOR_TIME            750
#define ALARM_TIME                350

/**
 * INPUTS & OUTPUTS
 */

Button ButtonIndicatorRight;
Button ButtonIndicatorLeft;
Button ButtonIndicatorAlarm;
Output LedRight;
Output LedLeft;
Output Buzzer;
LEDstrips LEDstrips;

/**
 * VARIABLES
 */

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

    pinMode(OUTPUT_BUZZER, OUTPUT);
    buzzer(false);

    LEDstrips.startup_animation();
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
        tone(OUTPUT_BUZZER, BUZZER_TIME);
    }
    else {
        noTone(OUTPUT_BUZZER);
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
    LEDstrips.loop();

    if (DEBUG) {
        Serial.print("\tINDICATOR: \t");  Serial.print(IndicatorState);
        Serial.print("\tALARM: \t");      Serial.print(AlarmState);
        Serial.println("");
    };

}


/**
 * LED STRIP CHOICES
 */

// void STRIPS_off() {
//   int x;
//   for(x=0; x<NUM_LEDS; x++){
//       left_leds[x] = CRGB(0,0,0);
//       right_leds[x] = CRGB(0,0,0);
//     }
// }



// void STRIP_indicater_toggle( int left_right ) {
//     CRGB color = CRGB::OrangeRed;
//     if (STRIP_indicater_state) {
//         CRGB color = CRGB(0,0,0);
//     }
//     STRIP_indicater_state = ! STRIP_indicater_state;
//     int i;
//     for(i=0; i < NUM_USED_LEDS; i++) {
//         if (left_right == -1) {
//             left_leds[NUM_LEDS-i-1] = color;
//             left_leds[i] = color;
//         }
//         if (left_right == 1) {
//             right_leds[NUM_LEDS-1-i] = color;
//             right_leds[i] = color;
//         }
//     }
// }

// void STRIP_indicater( int left_right ) {
//     if (left_right == 0) {
//         timer.cancel();
//         STRIP_indicater_state = false;
//     }
//     else {
//         STRIP_indicater_state = true;
//         timer.every(INDICATOR_TIME, STRIP_indicater_toggle,left_right);
//     }
// }




/**
 * Normal lightning:
 * Front LEDs white
 * Back LEDs red
 *
 * @param numberofleds [description]
 */
// void STRIPS_normal() {
//   int i;
//   for(i=0; i < NUM_USED_LEDS; i++){
//     left_leds[NUM_LEDS-i-1] = CRGB::Red;
//     left_leds[i] = CRGB::White;
//     right_leds[NUM_LEDS-1-i] = CRGB::Red;
//     right_leds[i] = CRGB::White;
//   }
// }
