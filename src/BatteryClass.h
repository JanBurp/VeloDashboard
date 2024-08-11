#pragma once

#include "Arduino.h"

#define VREF        3300
#define V_SAMPLES   60000 // filter out sudden changes in battery, take 1 minute
#define V_TIME      1

#define ADJUST_REF  4200
#define ADJUST      6.0 // on Test board should be 2.0


#define NUM_CELLS   3

#define BAT_BRIGHTNESS_ADJUST   0.5
#define BAT_BRIGHTNESS_OFF      0.25
#define BAT_LOW                 0.25
#define BAT_VERYLOW             0.20
#define BAT_ALMOSTDEAD          0.15
#define BAT_DEAD                0.10

#define POWER_OFF_DELAY 20 // seconds

class BatteryClass
{

private:
    byte pin;
    byte pinPower;
    float percentage;
    int cell_mv;
    unsigned long timeMeasurement;
    unsigned long timeDead;
    unsigned int battValues[V_SAMPLES] = {1023};
    unsigned int v_counter;

public:
    void init(byte pin, byte power)
    {
        this->pin = pin;
        this->pinPower = power;
        pinMode(this->pinPower, OUTPUT);
        digitalWrite(this->pinPower,LOW);
        this->cell_mv = 0;
        this->percentage = 1.0;
        this->timeMeasurement = millis();
        this->timeDead = 0;
        this->v_counter = 0;
        for (size_t v = 0; v < V_SAMPLES; v++)
        {
            int value = analogRead(this->pin);
            this->battValues[v] = value;
        }
    }

    void loop()
    {
        if ( (millis() - this->timeMeasurement) > V_TIME ) {
            this->timeMeasurement = millis();

            int value = analogRead(this->pin);
            this->battValues[this->v_counter] = value;
            this->v_counter ++;
            if ( this->v_counter>V_SAMPLES ) this->v_counter = 0;

            unsigned long totalValue = 0;
            for (size_t v = 0; v < V_SAMPLES; v++)
            {
                totalValue += this->battValues[v];
            }
            unsigned int meanValue = totalValue / V_SAMPLES;

            int battery_mv = 4.34 * (VREF / 1023) * meanValue;
            this->cell_mv = battery_mv / NUM_CELLS;
            // Adjust voltage to compensate for LiPo
            this->cell_mv = this->cell_mv - (ADJUST_REF - this->cell_mv) * ADJUST;
            if (this->cell_mv > ADJUST_REF) this->cell_mv = ADJUST_REF;

            const int V[21] = { 4200, 4150, 4110, 4080, 4020, 3980, 3950, 3910, 3870, 3850, 3840, 3820, 3800, 3790, 3770, 3750, 3730, 3710, 3690, 3610, 3270 };  // mV - 12.6 - 12.0 - 11.55 - 11.25 - 10.35 - 10.0
            const float P[21] = {  1.0,   0.95,   0.90,   0.85,   0.80,   0.75,   0.70,   0.65,   0.60,   0.55,   0.50,   0.45,   0.40,   0.35,   0.30,   0.25,   0.20,   0.15,   0.10,    0.5,    0.0 };  // % capacity

            float percent = 0.0;
            for (size_t i = 21; i > 1; i--) {
                if ( this->cell_mv >= V[i] ) {
                    percent = P[i] + (P[i-1] - P[i]) * (this->cell_mv - V[i]) / (V[i-1] - V[i]);
                }
            }

            if ( DEBUG && this->percentage != percent) {
                Serial.print(millis()/1000);
                Serial.print("\t\tmeanValue:\t");
                Serial.print(meanValue);
                Serial.print("\tmV:");
                Serial.print(battery_mv);
                Serial.print("\tmV:");
                Serial.print(this->cell_mv);
                Serial.print("\t%:");
                Serial.print(this->percentage);
                // Serial.print("\t%: [");
                // Serial.print(percent);
                // Serial.print("\t: ");
                // Serial.print(diff);

                // Serial.print("\ttmDead:");
                // Serial.print(this->timeDead);
                Serial.println();
            }

            this->percentage = percent;




        }

    }

    int getCellVoltage() {
        return this->cell_mv;
    }

    float getBatteryPercentage() {
        return this->percentage;
    }

    float getBrightnessPercentage() {
        float adjust = 1.0;
        if (this->percentage <= BAT_BRIGHTNESS_ADJUST) {
            adjust = map(this->percentage,BAT_BRIGHTNESS_OFF,BAT_BRIGHTNESS_ADJUST,0.0,1.0);
            if (adjust < 0) {
                adjust = 0;
            }
        }
        // if (DEBUG) {
        //     Serial.print(this->percentage);
        //     Serial.print("\t");
        //     Serial.println(adjust);
        // }
        return adjust;
    }

    bool isLow() {
        return this->percentage <= BAT_LOW;
    }

    bool isVeryLow() {
        return this->percentage <= BAT_VERYLOW;
    }

    bool isAlmostDead() {
        return this->percentage <= BAT_ALMOSTDEAD;
    }

    bool isDead() {
        if ( this->percentage<=BAT_DEAD ) {
            if ( this->timeDead == 0) {
                this->timeDead = millis();
            }
            return true;
        }
        return false;
    }

    int secondsUntilPowerOff() {
        int seconds = -1;
        if ( this->timeDead>0 ) {
            seconds = POWER_OFF_DELAY - ((millis() - this->timeDead) / 1000);
        }
        if ( seconds<0 ) seconds = 0;
        return seconds;
    }

    bool delayedPowerOff() {
        if ( this->timeDead>0 and this->secondsUntilPowerOff()==0 ) {
            return true;
        }
        return false;
    }

    void powerOff() {
        digitalWrite(this->pinPower,HIGH);
    }

};
