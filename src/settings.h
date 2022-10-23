#define DEBUG false           // [default = false] - set to true for Serial output
#define TEST true             // [default = false] - set to true for local test environment

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
#define DISPLAY_TIME 4

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
  #define NUM_LEDS            169 // 2,85 m * 66 = 188
  #define NUM_LIGHT_LEDS      36
  #define NUM_LIGHT_LEDS_BACK 48
  #define NUM_INDICATOR_LEDS  56
  #define BRIGHTNESS          128
  #define MAX_MILLIAMPS       1200
  #define FRAMES_PER_SECOND   100
#endif

// TEST:

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
#define PIN_BUZZER 12 // PWM
#define PIN_SPEED 11

#define PIN_INPUT_INDICATOR_LEFT 15
#define PIN_INPUT_INDICATOR_RIGHT 14

#define PIN_BUTTON_WHITE 23
#define PIN_BUTTON_RED 20
#define PIN_BUTTON_YELLOW_LEFT 22
#define PIN_BUTTON_YELLOW_RIGHT 21

#define PIN_INPUT_ALARM PIN_BUTTON_RED
#define PIN_INPUT_DISPLAY_SWITCH_LEFT PIN_BUTTON_YELLOW_LEFT
#define PIN_INPUT_DISPLAY_SWITCH_RIGHT PIN_BUTTON_YELLOW_RIGHT
