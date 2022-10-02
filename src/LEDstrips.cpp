#ifndef LEDstrips_h
#define LEDstrips_h

#include "Arduino.h"
#include "FastLED.h"

/**
 * LED STRIPS
 */

#define WELCOME_LENGTH      1

#define NUM_LEDS            169 // 2,85 m * 66 = 188
#define NUM_LIGHT_LEDS      36
#define NUM_LIGHT_LEDS_BACK 56
#define NUM_INDICATOR_LEDS  56
#define BRIGHTNESS          120
#define MAX_MILLIAMPS       1200
#define FRAMES_PER_SECOND   100

#define PIN_LEFT_STRIP      2
#define PIN_RIGHT_STRIP     3

#define LEFT    -1
#define BOTH    0
#define RIGHT   1



class LEDstrips {

    private:
        CRGB BLACK = CRGB(0,0,0);
        CRGB WHITE = CRGB(64,64,64);
        CRGB RED = CRGB(255,0,0);
        CRGB ORANGE = CRGB(255,128,0);
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

        void set(int strip, int start, int end, CRGB color) {
            for (int x = start; x < end; x++)
            {
                if (strip == LEFT || strip == BOTH)
                {
                    this->leds_left[x] = color;
                }
                if (strip == RIGHT || strip == BOTH)
                {
                    this->leds_right[x] = color;
                }
            }
        }

        void set_start_end(int strip, CRGB start_color, CRGB end_color, int num_leds = 0, CRGB between_color = CRGB(0, 0, 0) )
        {
            if (num_leds==0) {
                num_leds = NUM_LIGHT_LEDS;
            }
            for(int x=0; x < NUM_LEDS; x++){

                if ( x<=num_leds ) {
                    if (strip == LEFT || strip == BOTH)
                    {
                        this->leds_left[x] = start_color;
                    }
                    if (strip == RIGHT || strip == BOTH)
                    {
                        this->leds_right[x] = start_color;
                    }
                }

                if ( x>num_leds && x<(NUM_LEDS-num_leds) ) {
                    if (strip == LEFT || strip == BOTH) {
                        this->leds_left[x] = between_color;
                    }
                    if (strip == RIGHT || strip == BOTH) {
                        this->leds_right[x] = between_color;
                    }
                }

                if ( x >= (NUM_LEDS-num_leds)) {
                    if (strip == LEFT || strip == BOTH)
                    {
                        this->leds_left[x] = end_color;
                    }
                    if (strip == RIGHT || strip == BOTH)
                    {
                        this->leds_right[x] = end_color;
                    }
                }
            }
            FastLED.show();
        }

        void normal(int strip = BOTH) {
            this->blinkMs = 0;
            this->blinkState = false;
            this->set(strip,0,NUM_LIGHT_LEDS,WHITE);
            this->set(strip, NUM_LIGHT_LEDS, NUM_LEDS - NUM_LIGHT_LEDS_BACK, BLACK);
            this->set(strip, NUM_LEDS - NUM_LIGHT_LEDS_BACK, NUM_LEDS, RED);
            FastLED.show();
        }

        void blink(int strip, unsigned int periodMs = 500) {
            this->blinkStrip = strip;
            this->blinkMs = periodMs;
            this->blinkStartedMs = millis();
            this->blinkState = true;
            this->_blink_start();
        }

        void _blink_start() {
            this->set_all(this->blinkStrip,ORANGE);
            // this->set_start_end(this->blinkStrip,ORANGE,ORANGE,NUM_INDICATOR_LEDS,BLACK);
        }
        void _blink_stop() {
            this->set_all(this->blinkStrip, BLACK);
            // this->set_start_end(this->blinkStrip, BLACK, BLACK, NUM_INDICATOR_LEDS,BLACK);
        }

        // This uses delay, so stops all other actions...
        void startup_animation() {
            unsigned long delayMs = 200/NUM_LEDS;
            const int NUM_GRADIENT_LEDS = NUM_LEDS - 2*NUM_LIGHT_LEDS;
            CRGB colors[NUM_LEDS];
            CRGB gradient_colors[NUM_GRADIENT_LEDS];
            fill_gradient_RGB( gradient_colors, 0, WHITE, NUM_GRADIENT_LEDS, RED );
            for (int i = 0; i < NUM_LEDS; ++i)
            {
                if (i<=NUM_LIGHT_LEDS) {
                    colors[i] = WHITE;
                }
                else {
                    if (i>NUM_LIGHT_LEDS && i<(NUM_LEDS - NUM_LIGHT_LEDS)) {
                        colors[i] = gradient_colors[i - NUM_LIGHT_LEDS];
                    }
                    else {
                        colors[i] = RED;
                    }
                }
            }

            for (int t = 0; t < WELCOME_LENGTH; ++t)
            {
                this->off();
                int i;
                for (i = 0; i < NUM_LEDS; ++i) {
                    this->leds_left[i] = colors[i];
                    this->leds_right[i] = colors[i];
                    if (i>=NUM_LIGHT_LEDS) {
                        this->leds_left[i - NUM_LIGHT_LEDS] = BLACK;
                        this->leds_right[i - NUM_LIGHT_LEDS] = BLACK;
                    }
                    delay(delayMs);
                    FastLED.show();
                }
                for (i = NUM_LEDS - NUM_LIGHT_LEDS; i < NUM_LEDS; ++i) {
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

        unsigned int max_used_milliamps()
        {
            uint32_t left = calculate_unscaled_power_mW(this->leds_left, NUM_LEDS);
            uint32_t right = calculate_unscaled_power_mW(this->leds_right, NUM_LEDS);
            float current = (left + right) / 5.0;
            current = current * (BRIGHTNESS / 255.0);
            return (int) current;
        }
};

#endif
