#pragma once

#define DEBUG false  // [default = false] - set to true for Serial output
#define TEST false   // [default = false] - set to true for local test environment

/*
  WHEELS
*/

typedef struct
{
    char name[8];
    float circumference;
} Wheel;

// https://www.cateyeamerica.com/tire-size-calculator/
#define NR_ETRTO_WHEELS 13
Wheel Wheels[NR_ETRTO_WHEELS] = {
    {"23-406", 1.418},
    {"25-406", 1.431},
    {"26-406", 1.437},
    {"28-406", 1.450},
    {"30-406", 1.462},
    {"32-406", 1.475}, // CCU 32
    {"35-406", 1.494}, // Schwalbe Marathon+
    {"38-406", 1.512},
    {"40-406", 1.525},
    {"42-406", 1.538}, // CCU 42
    {"44-406", 1.550},
    {"50-406", 1.588},
    {"56-406", 1.625},
};

#define BIKE_DISTANCE_START         8060

typedef struct {
    unsigned long   timestamp;              // Time when data is stored
    unsigned long   totalDistance;
    float           tripDistance;
    float           dayDistance;
    unsigned long   dayTimeMovedSecs;
    float           dayAverageSpeed;
    float           dayMaxSpeed;
    float           prevDistance;
    float           prevAverageSpeed;
    float           prevMaxSpeed;
    float           wheelCircumference;
} MemoryStruct;
// total bytes = 4 * 11 = 44 bytes


// total bytes = 4 * 20 = 80 bytes
typedef struct {
    unsigned long   timestamp;                  // Time when data is stored

    unsigned long   totalDistance;              // ODO (meters)
    float           tripDistance1;              // sub total 1..3
    float           tripDistance2;
    float           tripDistance3;

    float           currentDistance;            // current trip
    unsigned long   currentStartTime;
    unsigned long   currentTime;
    float           currentAverageSpeed;
    float           currentMaxSpeed;

    float           dayDistance;                // day totals
    unsigned long   dayStartTime;
    unsigned long   dayTime;
    float           dayAverageSpeed;
    float           dayMaxSpeed;

    float           prevDistance;               // prev day
    unsigned long   prevTime;
    float           prevAverageSpeed;
    float           prevMaxSpeed;

    float           wheelCircumference;         // config
} newMemoryStruct;




/*
    LED light intensities
*/

#define HEAD_LED_OFF_INTENSITY      0
#define HEAD_LED_LOW_INTENSITY      16
#define HEAD_LED_MEDIUM_INTENSITY   92
#define HEAD_LED_MAX_INTENSITY      255

#define REAR_LED_OFF_INTENSITY      0
#define REAR_LED_LOW_INTENSITY      2
#define REAR_LED_MEDIUM_INTENSITY   32
#define REAR_LED_MAX_INTENSITY      128


/*
    TIMERS
*/
#define INDICATOR_TIMER             500ms
#define INDICATOR_TIMER_INT         500
#define INDICATOR_TIMER_STEP        50ms
#define ALARM_TIMER                 250ms


#if TEST
#define IDLE_TIMER 120
#define IDLE_WARNING_TIMER 20
#else
#define IDLE_TIMER 180           // seconds
#define IDLE_WARNING_TIMER 30    // seconds
#endif
#define SPEED_CALCULATION_TIMER 1000ms
#define BUZZER_TONE 660

/*
    DISPLAY
*/
#define SCREEN_WIDTH        128
#define SCREEN_HEIGHT       64
#define SCREEN_THIRD_HEIGHT 21
#define SCREEN_HALF_HEIGHT  32
#define SCREEN_HALF_WIDTH   64
#define SCREEN_HALF_HEIGHT_INFO 40
#define SCREEN_HALF_HEIGHT_VALUES 50

/*
  DISPLAY MODES
 */
#define DISPLAY_WELCOME     0
#define DISPLAY_HOME        1
#define DISPLAY_TODAY       2
#define DISPLAY_SPEEDS      3
#define DISPLAY_TOTALS      4
#define DISPLAY_TIME        5 // CHANGE WITH DISPLAY_TOTALS IF NEEDED
#define DISPLAY_PREV_DIST   6 // CHANGE WITH DISPLAY_TOTALS IF NEEDED
#define DISPLAY_PREV_SPEED  7 // CHANGE WITH DISPLAY_TOTALS IF NEEDED

#define DISPLAY_SETTINGS_TRIP   10
#define DISPLAY_SETTINGS_CLOCK   11
#define DISPLAY_SETTINGS_TYRE   12
#define DISPLAY_SETTINGS_TOTAL  13

/*
  LED STRIPS
*/

#define WELCOME_LENGTH 1

#define BRIGHTNESS 255
#define FRAMES_PER_SECOND 100
#if TEST
#define NUM_LEDS 12
#define NUM_LIGHT_LEDS 3
#define NUM_LIGHT_LEDS_BACK 3
#define NUM_INDICATOR_LEDS 3
#define MAX_MILLIAMPS 400
#else
#define NUM_LEDS 169 // 2,85 m * 66 = 188
#define NUM_LIGHT_LEDS 36
#define NUM_LIGHT_LEDS_BACK 48
#define NUM_INDICATOR_LEDS 56
#define MAX_MILLIAMPS 1500
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
#define PIN_LEFT_STRIP          2
#define PIN_RIGHT_STRIP         3
#define PIN_SPEED               4
#define PIN_POWER_OFF           5
#define PIN_BUZZER              6
// 7 - unused
#define PIN_HEAD_LIGHT_LEFT     8
#define PIN_HEAD_LIGHT_RIGHT    9
#define PIN_REAR_LIGHT          10
#define PIN_BRAKE_LIGHT         11
// 12 - unused

/*
 * PINS (north L-R)
 */
#define PIN_BATTERY_METER       23
#define PIN_DASHBOARD           22
// 21 - unused
// 20 - unused
// 19 - DISPLAY SCL
// 18 - DISPLAY SDA
// 17 - unused
// 16 - unused
#define PIN_TEST_SPEED          15              // 15 - unused, but used for testing
// #define PIN_HORN                14
#define INTERNAL_LED            13              // 13 - unused - OFF

/**
 * Unused PINS
 */
#if TEST
#define UNUSED_PINS         {0,1,7,12,14,16,17,20,21}
#define NR_UNUSED_PINS      9
#else
#define UNUSED_PINS         {0,1,7,12,14,15,16,17,20,21}
#define NR_UNUSED_PINS      10
#endif



