#pragma once

#include "Arduino.h"

#define VREF        3300
#define V_SAMPLES   30
#define V_TIME      1000

#define NUM_CELLS   3

#define BAT_LOW         20
#define BAT_VERYLOW     10
#define BAT_ALMOSTDEAD  7
#define BAT_DEAD        4

#define POWER_OFF_DELAY 20 // seconds

class BatteryClass
{

private:
    byte pin;
    byte pinPower;
    int percentage;
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
        this->percentage = 0;
        this->timeMeasurement = millis();
        this->timeDead = 0;
        this->v_counter = 0;
        for (size_t v = 0; v < V_SAMPLES; v++)
        {
            this->battValues[v] = 1023;
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

            int battery_mv = 4.37 * (VREF / 1023) * meanValue;
            this->cell_mv = battery_mv / NUM_CELLS;

            //
            // LiPo capacity
            //
            // const int V[6] = {4200, 4000, 3850, 3750, 3450, 3350};  // mV - 12.6 - 12.0 - 11.55 - 11.25 - 10.35 - 10.0
            // const int C[6] = {100,    90,   75,   50,    7,    0};  // % capacity

            const int V[2] = {4200, 3750};     // mV - 12.6 - 12.0 - 11.55 - 11.25 - 10.35 - 10.0
            const int C[2] = {100, 0};          // % capacity


            int percent = 0;
            for (size_t i = 1; i < 2; i++)
            {
                if ( this->cell_mv >= V[i] ) {
                    percent = int( (C[i-1] - C[i]) * (this->cell_mv - V[i]) / (V[i-1] - V[i]) + C[i] );
                }
            }
            this->percentage = constrain(percent, 0, 99);

            if ( DEBUG ) {
                Serial.print("value:\t");
                Serial.print(value);
                Serial.print("\t\tmeanValue:\t");
                Serial.print(meanValue);
                Serial.print("\tmV:");
                Serial.print(battery_mv);
                Serial.print("\tCells:");
                Serial.print(NUM_CELLS);
                Serial.print("\tmV:");
                Serial.print(this->cell_mv);
                Serial.print("\t%:");
                Serial.print(this->percentage);
                Serial.print("\ttmDead:");
                Serial.print(this->timeDead);
                Serial.println();
            }


        }

    }

    int getCellVoltage() {
        return this->cell_mv;
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
