#pragma once

#define DEBUG false // [default = false] - set to true for Serial output
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
  TIMERS
*/
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
#define BRIGHTNESS 10
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
 * PINS
 */
#define PIN_LEFT_STRIP 2
#define PIN_RIGHT_STRIP 3
#define PIN_DASHBOARD 23
#define PIN_BUZZER 12 // PWM
#define PIN_SPEED 11
