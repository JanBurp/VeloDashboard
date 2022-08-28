#ifndef LEDstrips_h
#define LEDstrips_h

#include "Arduino.h"
#include "FastLED.h"

/**
 * LED STRIPS
 */

#define NUM_LEDS            20
#define NUM_USED_LEDS       5
#define BRIGHTNESS          55
#define MAX_MILLIAMPS       800
#define FRAMES_PER_SECOND   100

#define PIN_RIGHT_STRIP     11
#define PIN_LEFT_STRIP      12

#define LEFT    -1
#define BOTH    0
#define RIGHT   1



class LEDstrips {

    private:
        CRGB BLACK = CRGB(0,0,0);
        CRGB WHITE = CRGB(255,255,255);
        CRGB RED = CRGB(255,0,0);
        CRGB ORANGE = CRGB(255,102,0);
        CRGB leds_left[NUM_LEDS], leds_right[NUM_LEDS];
        unsigned int blinkMs;
        unsigned long blinkStartedMs;
        int blinkStrip;
        bool blinkState;
        float blinkDuty;


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

        void off(int strip = BOTH) {
            this->set_all(strip,BLACK);
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

        void normal(int strip = BOTH) {
            this->set_start_end(strip,WHITE,RED);
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
            this->set_start_end(this->blinkStrip,ORANGE,ORANGE);
        }
        void _blink_stop() {
            this->set_start_end(this->blinkStrip,BLACK,BLACK);
        }

        // This uses delay, so stops all other actions...
        void startup_animation() {
            unsigned long delayMs = 500/NUM_LEDS;
            const int NUM_GRADIENT_LEDS = NUM_LEDS - 2*NUM_USED_LEDS;
            CRGB colors[NUM_LEDS];
            CRGB gradient_colors[NUM_GRADIENT_LEDS];
            fill_gradient_RGB( gradient_colors, 0, WHITE, NUM_GRADIENT_LEDS, RED );
            for (int i = 0; i < NUM_LEDS; ++i)
            {
                if (i<=NUM_USED_LEDS) {
                    colors[i] = WHITE;
                }
                else {
                    if (i>NUM_USED_LEDS && i<(NUM_LEDS - NUM_USED_LEDS)) {
                        colors[i] = gradient_colors[i - NUM_USED_LEDS];
                    }
                    else {
                        colors[i] = RED;
                    }
                }
            }

            for (int t = 0; t < 4; ++t)
            {
                this->off();
                int i;
                for (i = 0; i < NUM_LEDS; ++i) {
                    this->leds_left[i] = colors[i];
                    this->leds_right[i] = colors[i];
                    if (i>=NUM_USED_LEDS) {
                        this->leds_left[i - NUM_USED_LEDS] = BLACK;
                        this->leds_right[i - NUM_USED_LEDS] = BLACK;
                    }
                    delay(delayMs);
                    FastLED.show();
                }
                for (i = NUM_LEDS - NUM_USED_LEDS; i < NUM_LEDS; ++i) {
                    this->leds_left[i] = BLACK;
                    this->leds_right[i] = BLACK;
                    delay(delayMs);
                    FastLED.show();
                }
            }
            this->normal();
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
