#ifndef LEDstrips_h
#define LEDstrips_h

#include "Arduino.h"
#include "FastLED.h"

/**
 * LED STRIPS
 */

#define NUM_LEDS            20
#define NUM_USED_LEDS       5
#define BRIGHTNESS          60
#define MAX_MILLIAMPS       800
#define FRAMES_PER_SECOND   100

#define PIN_RIGHT_STRIP     11
#define PIN_LEFT_STRIP      12

#define LEFT    -1
#define BOTH    0
#define RIGHT   1

CRGB right_leds[NUM_LEDS];
CRGB left_leds[NUM_LEDS];


class LEDstrips {

    private:
        CRGB leds_left[NUM_LEDS], leds_right[NUM_LEDS];

        unsigned int blinkMs;
        unsigned long blinkStartedMs;
        int blinkStrip;
        bool blinkState;
        float blinkDuty;
        // unsigned long lastOnMs;

        // unsigned int minDurationMs;
        // bool state;
        // bool stateHardware;
        // unsigned int blinkMs;
        // unsigned long blinkStartedMs;


	public:

		LEDstrips() {
            FastLED.addLeds<WS2812B, PIN_LEFT_STRIP, GRB>(this->leds_left, NUM_LEDS);
            FastLED.addLeds<WS2812B, PIN_RIGHT_STRIP, GRB>(this->leds_right, NUM_LEDS);
            FastLED.setBrightness(BRIGHTNESS);
            FastLED.setMaxPowerInVoltsAndMilliamps(5,MAX_MILLIAMPS);
		}

        void set_all(int strip, CRGB color) {
            for(int x=0; x < NUM_LEDS; x++){
                if ( strip == LEFT || strip == BOTH) {
                    this->leds_left[x] = color;
                }
                if ( strip == RIGHT || strip == BOTH) {
                    this->leds_right[x] = color;
                }
            }
            FastLED.show();
        }

        void off(int strip) {
            this->set_all(strip,CRGB(0,0,0));
            this->blinkMs = 0;
            this->blinkState = false;
        }


        void set_start_end(int strip, CRGB start_color, CRGB end_color) {
            for(int x=0; x < NUM_USED_LEDS; x++){
                if ( strip == LEFT || strip == BOTH) {
                    this->leds_left[x] = start_color;
                    this->leds_left[NUM_LEDS - x - 1] = end_color;
                }
                if ( strip == RIGHT || strip == BOTH) {
                    this->leds_right[x] = start_color;
                    this->leds_right[NUM_LEDS - x - 1] = end_color;
                }
            }
            FastLED.show();
        }

        void normal(int strip) {
            this->set_start_end(strip,CRGB::White,CRGB::Red);
            this->blinkMs = 0;
            this->blinkState = false;
        }

        void blink(int strip, unsigned int periodMs = 500) {
            this->blinkStrip = strip;
            this->blinkMs = periodMs;
            this->blinkStartedMs = millis();
            this->blinkState = true;
            this->_blink_start();
        }

        void _blink_start() {
            this->set_start_end(this->blinkStrip,CRGB::Orange,CRGB::Orange);
        }
        void _blink_stop() {
            this->set_start_end(this->blinkStrip,CRGB(0,0,0),CRGB(0,0,0));
        }

		/**
		 * Call this in the main loop.
		 */
		void loop() {
			if (this->blinkMs > 0) {
				unsigned long t = ((millis() - this->blinkStartedMs) % this->blinkMs);
				this->blinkState = (t < this->blinkMs/2);
                if (this->blinkState) {
                    this->_blink_start();
                }
                else {
                    this->_blink_stop();
                }
			}
		}

		// void on() {
		// 	this->set(true);
		// }

		// void off() {
		// 	this->set(false);
		// }

		// void toggle() {
		// 	this->set(!state);
		// }

  //       bool getState() {
  //           return this->stateHardware;
  //       }

		// /**
		//  * Turn on the OUTPUT, then turn it off immediately.
		//  * A single impulse of light will be visible if OUTPUT's minDurationMs is long enough.
		//  */
		// void flash() {
		// 	this->set(true);
		// 	this->set(false);
		// }
};

#endif
