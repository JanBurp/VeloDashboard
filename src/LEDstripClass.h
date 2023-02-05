#pragma once

#include "Arduino.h"
#include "FastLED.h"
#include "settings.h"
#include "LightsClass.h"
#include "BatteryClass.h"
#include "IdleClass.h"

PeriodicTimer stripTimer(TCK);

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
    CRGB leds_left[NUM_LEDS], leds_right[NUM_LEDS];
    bool turnedOff = false;
    int indicatorStrip = 0;
    unsigned int indicatorTimer = 0;
    IndicatorClass *Indicators;
    LightsClass *Lights;
    BatteryClass *Battery;
    IdleClass *IdleTimer;

public:
    LEDstripClass()
    {
        FastLED.addLeds<WS2812B, PIN_LEFT_STRIP, GRB>(this->leds_left, NUM_LEDS);
        FastLED.addLeds<WS2812B, PIN_RIGHT_STRIP, GRB>(this->leds_right, NUM_LEDS);
        FastLED.setBrightness(BRIGHTNESS);
        FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MILLIAMPS);
    }

    void init(IndicatorClass *indicators, LightsClass *lights, BatteryClass *battery, IdleClass *idle)
    {
        this->Indicators = indicators;
        this->Lights = lights;
        this->Battery = battery;
        this->IdleTimer = idle;
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

    void set(int strip, int start, int end, CRGB color)
    {
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

    void set_start_end(int strip, CRGB start_color, CRGB end_color, int num_leds = 0, CRGB between_color = CRGB(0, 0, 0))
    {
        if (num_leds == 0)
        {
            num_leds = NUM_LIGHT_LEDS;
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
        // if (DEBUG)
        // {
        //     Serial.print("START");
        //     Serial.println();
        // }
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
        int num_leds = int(NUM_LEDS / 2 * percentage) + 10;
        if (TEST) {
            num_leds -= 10;
        }

        // if (DEBUG)
        // {
        //     Serial.print("BLINK");
        //     Serial.print("\tPercentage:\t");
        //     Serial.print(percentage);
        //     Serial.print("\tLEDS:\t");
        //     Serial.print(num_leds);
        //     Serial.println();
        // }

        this->set(this->indicatorStrip, 0, num_leds, ORANGE);
        this->set(this->indicatorStrip, num_leds, NUM_LEDS - num_leds, BLACK);
        this->set(this->indicatorStrip, NUM_LEDS - num_leds, NUM_LEDS, ORANGE);

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
        // if (DEBUG)
        // {
        //     Serial.print("STOP - ");
        //     Serial.print(this->indicatorStrip);
        //     Serial.println();
        // }
        stripTimer.stop();
        this->set_all(this->indicatorStrip, BLACK);
        this->indicatorTimer = 0;
        this->indicatorStrip = 0;
    }

    void normal(int strip = BOTH)
    {
        CRGB white = WHITE_DIM;
        CRGB red = RED_DIM;
        if (this->Lights->getLights() >= LIGHTS_DIM)
        {
            white = WHITE;
            red = RED;
        }
        if (this->Lights->getBrake())
        {
            red = RED_FULL;
        }

        int num_light_leds = NUM_LIGHT_LEDS;
        int num_light_leds_back = NUM_LIGHT_LEDS_BACK;
        if (this->IdleTimer->warning())
        {
            num_light_leds = int(num_light_leds * this->IdleTimer->remainingPercentage());
            num_light_leds_back = int(num_light_leds_back * this->IdleTimer->remainingPercentage());
        }

        this->set(strip, 0, num_light_leds, white);
        this->set(strip, num_light_leds, NUM_LEDS - num_light_leds_back, BLACK);
        this->set(strip, NUM_LEDS - num_light_leds_back, NUM_LEDS, red);

        FastLED.show();
    }

    /**
     * Call this in the main loop.
     */
    void loop()
    {
        if (this->Battery->isVeryLow())
        {
            FastLED.setBrightness(BRIGHTNESS * 0.5);
        }
        if (this->Battery->isAlmostDead())
        {
            FastLED.setBrightness(BRIGHTNESS * 0.1);
        }

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
        unsigned long delayMs = 200 / NUM_LEDS;
        const int NUM_GRADIENT_LEDS = NUM_LEDS - 2 * NUM_LIGHT_LEDS;
        CRGB colors[NUM_LEDS];
        CRGB gradient_colors[NUM_GRADIENT_LEDS];
        fill_gradient_RGB(gradient_colors, 0, WHITE, NUM_GRADIENT_LEDS, RED);
        for (int i = 0; i < NUM_LEDS; ++i)
        {
            if (i <= NUM_LIGHT_LEDS)
            {
                colors[i] = WHITE;
            }
            else
            {
                if (i > NUM_LIGHT_LEDS && i < (NUM_LEDS - NUM_LIGHT_LEDS))
                {
                    colors[i] = gradient_colors[i - NUM_LIGHT_LEDS];
                }
                else
                {
                    colors[i] = RED;
                }
            }
        }

        for (int t = 0; t < WELCOME_LENGTH; ++t)
        {
            this->set_all(BOTH, BLACK);
            int i;
            for (i = 0; i < NUM_LEDS; ++i)
            {
                this->leds_left[i] = colors[i];
                this->leds_right[i] = colors[i];
                if (i >= NUM_LIGHT_LEDS)
                {
                    this->leds_left[i - NUM_LIGHT_LEDS] = BLACK;
                    this->leds_right[i - NUM_LIGHT_LEDS] = BLACK;
                }
                delay(delayMs);
                FastLED.show();
            }
            for (i = NUM_LEDS - NUM_LIGHT_LEDS; i < NUM_LEDS; ++i)
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
