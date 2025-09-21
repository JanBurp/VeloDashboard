#pragma once

#include "Arduino.h"
#include "FastLED.h"
#include "settings.h"
#include "LightsClass.h"
#include "BatteryClass.h"
#include "IdleClass.h"

PeriodicTimer stripTimer(TCK);

CRGB gradient_colors[NUM_LEDS];

/**
 * LED STRIPS
 */

class LEDstripClass
{

private:
    CRGB BLACK = CRGB(0, 0, 0);
    CRGB WHITE_DIM = CRGB(12, 12, 12);
    CRGB WHITE = CRGB(52, 52, 52);
    CRGB WHITE_FULL = CRGB(128, 128, 128);
    CRGB RED_DIM = CRGB(32, 0, 0);
    CRGB RED = CRGB(64, 0, 0);
    CRGB RED_FULL = CRGB(255, 0, 0);
    CRGB ORANGE = CRGB(255, 128, 0);
    // CRGB BLUE = CRGB(0, 0, 128);

    CRGB leds_left[NUM_LEDS], leds_right[NUM_LEDS];
    bool turnedOff = false;
    int indicatorStrip = 0;
    unsigned int indicatorTimer = 0;
    IndicatorClass *Indicators;
    LightsClass *Lights;
    BatteryClass *Battery;
    IdleClass *IdleTimer;
    SpeedClass *Speed;

public:
    LEDstripClass()
    {
        FastLED.addLeds<WS2812B, PIN_LEFT_STRIP, GRB>(this->leds_left, NUM_LEDS);
        FastLED.addLeds<WS2812B, PIN_RIGHT_STRIP, GRB>(this->leds_right, NUM_LEDS);
        FastLED.setBrightness(BRIGHTNESS);
        FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MILLIAMPS);
        fill_gradient_RGB(gradient_colors, 0, WHITE, NUM_LEDS, RED);
    }

    void init(IndicatorClass *indicators, LightsClass *lights, BatteryClass *battery, IdleClass *idle, SpeedClass *speed)
    {
        this->Indicators = indicators;
        this->Lights = lights;
        this->Battery = battery;
        this->IdleTimer = idle;
        this->Speed = speed;
    }

    void set_all(int strip, CRGB color)
    {
        this->_set_all(strip, color);
        FastLED.show();
    }

    void _set_all(int strip, CRGB color)
    {
        for (int x = 0; x < NUM_LEDS; x++)
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

    void off(int strip = BOTH)
    {
        if (!this->turnedOff)
        {
            this->set_all(strip, BLACK);
            FastLED.show();
            this->turnedOff = true;
        }
    }

    void set(int strip, int start, int end, CRGB color, bool dotted = false)
    {
        bool dot = true;
        for (int x = start; x < end; x++)
        {
            if (dot || !dotted) {
                if (strip == LEFT || strip == BOTH)
                {
                    this->leds_left[x] = color;
                }
                if (strip == RIGHT || strip == BOTH)
                {
                    this->leds_right[x] = color;
                }
            }
            else {
                this->leds_left[x] = BLACK;
                this->leds_right[x] = BLACK;
            }
            dot = ! dot;
        }
    }

    void set_start_end(int strip, CRGB start_color, CRGB end_color, int num_leds = 0, CRGB between_color = CRGB(0, 0, 0))
    {
        if (num_leds == 0)
        {
            num_leds = NUM_LIGHT_LEDS_FRONT;
        }
        for (int x = 0; x < NUM_LEDS; x++)
        {

            if (x <= num_leds)
            {
                if (strip == LEFT || strip == BOTH)
                {
                    this->leds_left[x] = start_color;
                }
                if (strip == RIGHT || strip == BOTH)
                {
                    this->leds_right[x] = start_color;
                }
            }

            if (x > num_leds && x < (NUM_LEDS - num_leds))
            {
                if (strip == LEFT || strip == BOTH)
                {
                    this->leds_left[x] = between_color;
                }
                if (strip == RIGHT || strip == BOTH)
                {
                    this->leds_right[x] = between_color;
                }
            }

            if (x >= (NUM_LEDS - num_leds))
            {
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

    void blink_animation_start(int strip = BOTH)
    {
        this->indicatorStrip = strip;
        if (this->indicatorTimer == 0)
        {
            this->indicatorTimer = millis();
            stripTimer.begin([this] { this->blink_animation(); }, INDICATOR_TIMER_STEP);
            this->blink_animation();
        }
    }

    void blink_animation()
    {
        float percentage = float(millis() - this->indicatorTimer) / float(INDICATOR_TIMER_INT);
        int num_leds_front = int(NUM_INDICATOR_LEDS * percentage);
        int num_leds_back = int(NUM_INDICATOR_LEDS * percentage);

        int start_leds = 0;
        int end_leds = NUM_LEDS;

        this->set(this->indicatorStrip, 0, start_leds, BLACK);
        this->set(this->indicatorStrip, start_leds, num_leds_front, ORANGE);
        this->set(this->indicatorStrip, num_leds_front, NUM_LEDS - num_leds_back, BLACK);
        this->set(this->indicatorStrip, NUM_LEDS - num_leds_back, end_leds, ORANGE);
        this->set(this->indicatorStrip, end_leds, NUM_LEDS, BLACK);

        if ((millis() - this->indicatorTimer) >= INDICATOR_TIMER_INT)
        {
            this->blink_animation_stop();
        }
        else
        {
            FastLED.show();
        }
    }

    bool is_blink_animation_started()
    {
        return (this->indicatorTimer > 0);
    }

    void blink_animation_stop()
    {
        stripTimer.stop();
        this->set_all(this->indicatorStrip, BLACK);
        this->indicatorTimer = 0;
        this->indicatorStrip = 0;
    }

    void normal(int strip = BOTH)
    {
        CRGB white = BLACK;
        CRGB red = BLACK;

        switch (this->Lights->getLights())
        {
            case LIGHTS_OFF:
                white = BLACK;
                red = BLACK;
                break;
            case LIGHTS_DIM:
                white = WHITE_DIM;
                red = RED_DIM;
                break;
            case LIGHTS_ON:
            case LIGHTS_NORMAL:
            case LIGHTS_BEAM:
                white = WHITE;
                red = RED;
                break;
            case LIGHTS_FOG:
                white = WHITE;
                red = RED_FULL;
                break;
        }

        if (this->Lights->getBrake())
        {
            red = RED_FULL;
        }

        int numLEDSfront = NUM_LIGHT_LEDS_FRONT;
        int numLEDSback = NUM_LIGHT_LEDS_BACK;
        if (this->IdleTimer->warning())
        {
            numLEDSfront = int(NUM_LIGHT_LEDS_FRONT * this->IdleTimer->remainingPercentage());
            numLEDSback = int(NUM_LIGHT_LEDS_BACK * this->IdleTimer->remainingPercentage());
        }

        this->set(strip, 0, numLEDSfront,white,true);
        this->set(strip, numLEDSfront, NUM_LEDS - numLEDSback, BLACK);
        this->set(strip, NUM_LEDS - numLEDSback, NUM_LEDS, red);


        // SPEED INDICATOR - TODO
        // if ( !this->Speed->isPaused() ) {
        //     float dist = this->Speed->getCurrentDistance() * 1000; // dist in meter
        //     float bike_dist = (dist*100) / float(BIKE_LENGTH);
        //     int led_nr = int(bike_dist * float(NUM_LEDS)) % NUM_LEDS;
        //     CRGB color = gradient_colors[led_nr];
        //     if (this->Lights->getLights() > 0) {
        //         if (led_nr < NUM_LIGHT_LEDS_FRONT || led_nr > (NUM_LEDS - NUM_LIGHT_LEDS_BACK)) {
        //             color = BLACK;
        //         }
        //     }
        //     this->set(strip, led_nr, led_nr + NUM_SPEED_LEDS, color);
        // }

        FastLED.show();
    }

    /**
     * Call this in the main loop.
     */
    void loop()
    {
        FastLED.setBrightness(int(BRIGHTNESS * this->Battery->getBrightnessPercentage()));

        if ( !this->Indicators->isActive() )
        {
            this->normal(BOTH);
            if (this->is_blink_animation_started())
            {
                this->blink_animation_stop();
            }
        }
        else
        {
            if (this->Indicators->getStateLeft())
            {
                this->normal(RIGHT);
                if (!this->is_blink_animation_started())
                {
                    this->blink_animation_start(LEFT);
                }
            }
            if (this->Indicators->getStateRight())
            {
                this->normal(LEFT);
                if (!this->is_blink_animation_started())
                {
                    this->blink_animation_start(RIGHT);
                }
            }
        }
    }

    // This uses delay, so stops all other actions...
    void startup_animation()
    {
        unsigned long delayMs = WELCOME_ANIMATION / NUM_LEDS;

        for (int t = 0; t < WELCOME_LENGTH; ++t)
        {
            this->set_all(BOTH, BLACK);
            int i;
            for (i = 0; i < NUM_LEDS; ++i)
            {
                this->leds_left[i] = gradient_colors[i];
                this->leds_right[i] = gradient_colors[i];
                if (i >= NUM_LIGHT_LEDS_FRONT)
                {
                    this->leds_left[i - NUM_LIGHT_LEDS_FRONT] = BLACK;
                    this->leds_right[i - NUM_LIGHT_LEDS_FRONT] = BLACK;
                }
                delay(delayMs);
                FastLED.show();
            }
            for (i = NUM_LEDS - NUM_LIGHT_LEDS_FRONT; i < NUM_LEDS; ++i)
            {
                this->leds_left[i] = BLACK;
                this->leds_right[i] = BLACK;
                delay(delayMs);
                FastLED.show();
            }
        }
        this->normal();
    }

    unsigned int max_used_milliamps()
    {
        uint32_t left = calculate_unscaled_power_mW(this->leds_left, NUM_LEDS);
        uint32_t right = calculate_unscaled_power_mW(this->leds_right, NUM_LEDS);
        float current = (left + right) / 5.0;
        current = current * (BRIGHTNESS / 255.0);
        return (int)current;
    }
};
