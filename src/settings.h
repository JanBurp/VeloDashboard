#pragma once

#define DEBUG true // [default = false] - set to true for Serial output
#define TEST true   // [default = false] - set to true for local test environment

/*
  WHEELS
*/

typedef struct
{
    char name[8];
    float circumference;
} Wheel;

Wheel Wheels[7] = {
    {"23-406", 1.420},
    {"28-406", 1.450}, // Schwalbe Marathon+
    {"35-406", 1.510},
    {"40-406", 1.540},
    {"47-406", 1.580},
    {"50-406", 1.600},
    {"54-406", 1.620},
};

// Change this for you're wheel
unsigned int WheelNumber = 1;

/*
    LED light intensities
*/

#define HEAD_LED_OFF_INTENSITY      0
#define HEAD_LED_LOW_INTENSITY      16
#define HEAD_LED_MEDIUM_INTENSITY   96
#define HEAD_LED_MAX_INTENSITY      255

#define REAR_LED_OFF_INTENSITY      0
#define REAR_LED_LOW_INTENSITY      2
#define REAR_LED_MEDIUM_INTENSITY   32
#define REAR_LED_MAX_INTENSITY      128


/*
    HORN Loudness
*/
#define HORN_LOUDNESS               10 // 10 = 6.2V // 11 = 7.3V // 12 = 8.3V // 13 = 9V ...

/*
    TIMERS
*/
#if TEST
#define IDLE_TIMER 120
#define IDLE_WARNING_TIMER 20
#else
#define IDLE_TIMER 300           // seconds
#define IDLE_WARNING_TIMER 60    // seconds
#endif
#define SPEED_CALCULATION_TIMER 1000ms
#define BUZZER_TONE 660

/*
    DISPLAY
*/
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_HALF_HEIGHT 32
#define SCREEN_HALF_WIDTH 64
#define SCREEN_HALF_HEIGHT_INFO 40
#define SCREEN_HALF_HEIGHT_VALUES 50

/*
  DISPLAY MODES
 */
#define DISPLAY_WELCOME 0
#define DISPLAY_SPEED_AND_TIME 1
#define DISPLAY_SPEEDS 2
#define DISPLAY_DISTANCE 3
#define DISPLAY_TRIPTIME 4

/*
  LED STRIPS
*/

#define WELCOME_LENGTH 1

#if TEST
#define NUM_LEDS 12
#define NUM_LIGHT_LEDS 5
#define NUM_LIGHT_LEDS_BACK 5
#define NUM_INDICATOR_LEDS 7
#define BRIGHTNESS 128
#define MAX_MILLIAMPS 40
#define FRAMES_PER_SECOND 100
#else
#define NUM_LEDS 169 // 2,85 m * 66 = 188
#define NUM_LIGHT_LEDS 36
#define NUM_LIGHT_LEDS_BACK 48
#define NUM_INDICATOR_LEDS 56
#define BRIGHTNESS 128
#define MAX_MILLIAMPS 1200
#define FRAMES_PER_SECOND 100
#endif

#define LEFT -1
#define BOTH 0
#define RIGHT 1

/*
  Speed
*/

#define SENSOR_BUFF 12
#define MIN_SENSOR_TIME 75
#define MAX_SENSOR_TIME 3000
#define MIN_SPEED 2

/*
 * PINS (south L->R)
 */

// 0 - unused
// 1 - unused
#define PIN_LEFT_STRIP      2
#define PIN_RIGHT_STRIP     3
#define PIN_SPEED           4              // LET OP VERHUIST !!!!
#define PIN_POWER_OFF       5              // VERHUIST
#define PIN_BUZZER          6              // VERHUIST PWM
// 7 - unused
#define PIN_HEAD_LIGHT_LEFT     8
#define PIN_HEAD_LIGHT_RIGHT    9
#define PIN_REAR_LIGHT          10
#define PIN_BRAKE_LIGHT         11
// 12 - unused

/*
 * PINS (north L-R)
 */
#define PIN_BATTERY_METER   23             // VERHUISD
#define PIN_DASHBOARD       22             // VERHUISD
// 21 - unused
// 20 - unused
// 19 - DISPLAY SCL
// 18 - DISPLAY SDA
// 17 - unused
// 16 - unused
// 15 - unused
#define PIN_HORN            14
// 13 - unused

/**
 * Unused PINS
 */
#define UNUSED_PINS         {0,1,7,12,13,15,16,17,20,21}
#define NR_UNUSED_PINS      10
