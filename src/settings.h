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


#if TEST
#define BIKE_LENGTH                 15 // cm
#else
#define BIKE_LENGTH                 285 // cm
#endif



#define BIKE_DISTANCE_START         8060
#define MAX_TIME_SHORT_BRAKE        21600 // 6 hours = 60*60*6 seconds

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
} MemoryStruct;




/*
    LED light intensities
*/

#define HEAD_LED_OFF_INTENSITY      0
#define HEAD_LED_DIM_INTENSITY      0
#define HEAD_LED_ON_INTENSITY       4   // 4 = minimum light
#define HEAD_LED_NORMAL_INTENSITY   92
#define HEAD_LED_BEAM_INTENSITY     140 // 140 = maximum light
#define HEAD_LED_FOG_INTENSITY      140

#define REAR_LED_OFF_INTENSITY      0
#define REAR_LED_DIM_INTENSITY      4
#define REAR_LED_ON_INTENSITY       16
#define REAR_LED_NORMAL_INTENSITY   32
#define REAR_LED_BEAM_INTENSITY     64
#define REAR_LED_FOG_INTENSITY      140 // 140 = max

#define BLINK_OFF                   0
#define BLINK_ON                    140

#define BRAKE_LEDS_OFF              0
#define BRAKE_LEDS_FOG              64
#define BRAKE_LEDS_ON               140



/*
    TIMERS
*/
#define INDICATOR_TIMER             350ms
#define INDICATOR_TIMER_INT         350
#define INDICATOR_TIMER_STEP        35ms
#define ALARM_TIMER                 350ms
#if TEST
#define WELCOME_ANIMATION           250
#else
#define WELCOME_ANIMATION           250
#endif

#if TEST
#define IDLE_TIMER 120
#define IDLE_WARNING_TIMER 20
#else
#define IDLE_TIMER 180           // seconds
#define IDLE_WARNING_TIMER 30    // seconds
#endif
#define SPEED_CALCULATION_TIMER 1000ms
#define SPEED_CALCULATION_TIME  1000
#define MINUTE 60000
#define BUZZER_TONE 440

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
#define DISPLAY_CURRENT     2
#define DISPLAY_TODAY       3
#define DISPLAY_PREV        4
#define DISPLAY_TOTALS      5

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
#define NUM_LEDS                12
#define NUM_LIGHT_LEDS_FRONT    3
#define NUM_LIGHT_LEDS_BACK     3
#define NUM_INDICATOR_LEDS      3
#define NUM_SPEED_LEDS          1
#define MAX_MILLIAMPS 400
#else
#define NUM_LEDS                169 // 2,85 m * 66 = 188
#define NUM_LIGHT_LEDS_FRONT    36
#define NUM_LIGHT_LEDS_BACK     48
#define NUM_INDICATOR_LEDS      70
#define NUM_SPEED_LEDS          3
#define MAX_MILLIAMPS           2000
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

// 0 - unused -> extra LEDstrips
// 1 - unused -> extra LEDstrips
#define PIN_LEFT_STRIP          2
#define PIN_RIGHT_STRIP         3
// 4 - unused -> addon
#define PIN_POWER_OFF           5
#define PIN_BLINK_LEFT          6
#define PIN_BLINK_RIGHT         7
#define PIN_REAR_LIGHT          8
#define PIN_BRAKE_LIGHT         9
#define PIN_HEAD_LIGHT_LEFT     10
#define PIN_HEAD_LIGHT_RIGHT    11
#define PIN_BUZZER              12

/*
 * PINS (north L-R)
 */
#define DASHBOARD_BREAK         23
#define DASHBOARD_BUTTONS_LEFT  22
#define DASHBOARD_BUTTONS_RIGHT 21
#define DASHBOARD_LED_RIGHT     19
#define DASHBOARD_LED_LEFT      20
#define PIN_BATTERY_METER       18
// 17 - DISPLAY SCL
// 16 - DISPLAY SDA
#define PIN_CADANS              15              // 15 - unused
#define PIN_SPEED               14
#define INTERNAL_LED            13              // 13 - unused - OFF

/**
 * Unused PINS
 */
#define UNUSED_PINS         {0,1,4,15, 16,17}
#define NR_UNUSED_PINS      6
