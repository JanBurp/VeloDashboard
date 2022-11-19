#pragma once

#include "Arduino.h"

#define VREF        3300
#define V_SAMPLES   10
#define NUM_CELLS   3

#define BAT_LOW         20
#define BAT_VERYLOW     10
#define BAT_ALMOSTDEAD  7
#define BAT_DEAD        5

class BatteryClass
{

private:
    byte pin;
    int percentage;

public:
    void init(byte pin)
    {
        this->pin = pin;
        this->percentage = 0;
    }

    void loop()
    {
        int value = analogRead(this->pin);
        static int meanValue = value;
        meanValue = (meanValue * (V_SAMPLES - 1) + value) / V_SAMPLES;
        int battery_mv = 3.875 * (VREF / 1023) * meanValue;
        int cell_mv = battery_mv / NUM_CELLS;

        //
        // LiPo capacity
        //
        const int V[6] = {4200, 4000, 3850, 3750, 3450, 3350};  //mV
        const int C[6] = {100,    90,   75,   50,    7,    0};  // % capacity

        int prc = 0;
        for (size_t i = 1; i < 5; i++)
        {
            if ( cell_mv >= V[i] ) {
                prc = int( (C[i-1] - C[i]) * (cell_mv - V[i]) / (V[i-1] - V[i]) + C[i] );
            }
        }
        this->percentage = constrain(prc, 0, 99);

        if ( DEBUG ) {
            Serial.print("Value:\t");
            Serial.print(value);
            Serial.print("\tmV:");
            Serial.print(battery_mv);
            Serial.print("\tCells:");
            Serial.print(NUM_CELLS);
            Serial.print("\tmV:");
            Serial.print(cell_mv);
            Serial.print("\t%:");
            Serial.print(this->percentage);
            Serial.println();
        }

    }

    int getBatteryPercentage() {
        return this->percentage;
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
        return this->percentage <= BAT_DEAD;
    }



};
