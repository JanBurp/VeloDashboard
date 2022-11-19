#pragma once

#include "Arduino.h"
#include "FastLED.h"
#include "settings.h"
#include "BatteryClass.h"

/**
 * LED STRIPS
 */

class LEDstripClass
{

private:
    CRGB BLACK = CRGB(0, 0, 0);
    CRGB WHITE = CRGB(52, 52, 52);
    CRGB BRIGHT_WHITE = CRGB(255, 255, 255);
    CRGB RED = CRGB(255, 0, 0);
    CRGB ORANGE = CRGB(255, 128, 0);
    CRGB leds_left[NUM_LEDS], leds_right[NUM_LEDS];
    bool blinkLeft = false;
    bool blinkRight = false;
    bool turnedOff = false;
    IndicatorClass *Indicators;
    BatteryClass *Battery;

public:
    LEDstripClass()
    {
        FastLED.addLeds<WS2812B, PIN_LEFT_STRIP, GRB>(this->leds_left, NUM_LEDS);
        FastLED.addLeds<WS2812B, PIN_RIGHT_STRIP, GRB>(this->leds_right, NUM_LEDS);
        FastLED.setBrightness(BRIGHTNESS);
        FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MILLIAMPS);
    }

    void init(IndicatorClass *indicators, BatteryClass *battery)
    {
        this->Indicators = indicators;
        this->Battery = battery;
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
        if ( !this->turnedOff ) {
            this->set_all(strip, BLACK);
            this->blinkLeft = false;
            this->blinkRight = false;
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

    void normal(int strip = BOTH)
    {
        this->blinkLeft = false;
        this->blinkRight = false;
        this->set(strip, 0, 3, BRIGHT_WHITE);
        this->set(strip, 3, NUM_LIGHT_LEDS, WHITE);
        this->set(strip, NUM_LIGHT_LEDS, NUM_LEDS - NUM_LIGHT_LEDS_BACK, BLACK);
        this->set(strip, NUM_LEDS - NUM_LIGHT_LEDS_BACK, NUM_LEDS, RED);
        FastLED.show();
    }

    /**
     * Call this in the main loop.
     */
    void loop()
    {
        if ( this->Battery->isVeryLow() ) {
            FastLED.setBrightness(BRIGHTNESS * 0.5);
        }
        if ( this->Battery->isAlmostDead() ) {
            FastLED.setBrightness(BRIGHTNESS * 0.1);
        }

        if (!this->Indicators->isActive())
        {
            this->normal(BOTH);
        }
        else
        {
            bool show = false;

            if (this->Indicators->getStateLeft())
            {
                if (!this->blinkLeft)
                {
                    this->_set_all(LEFT, ORANGE);
                    show = true;
                }
            }
            else
            {
                if (this->blinkLeft)
                {
                    this->_set_all(LEFT, BLACK);
                    show = true;
                }
            }

            if (this->Indicators->getStateRight())
            {
                if (!this->blinkRight)
                {
                    this->_set_all(RIGHT, ORANGE);
                    show = true;
                }
            }
            else
            {
                if (this->blinkRight)
                {
                    this->_set_all(RIGHT, BLACK);
                    show = true;
                }
            }

            if (show)
            {
                FastLED.show();
            }
        }

        this->blinkLeft = this->Indicators->getStateLeft();
        this->blinkRight = this->Indicators->getStateRight();
    }

    // This uses delay, so stops all other actions...
    void startup_animation()
    {
        this->blinkLeft = false;
        this->blinkRight = false;

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
            this->set_all(BOTH,BLACK);
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
