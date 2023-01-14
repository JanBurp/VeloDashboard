#pragma once

#include "Arduino.h"
#include <EEPROM.h>

#define ODO_ADDRESS 0
#define MEM_ADDRESS 4

class SpeedClass
{

private:
    bool started = false;
    bool paused = true;
    unsigned long pausedTime = 0;

    MemoryStruct Memory;

    // Speed sensor
    bool SpeedSensor = true;
    volatile unsigned long lastSensorTimeMs = 0;
    volatile unsigned long sensorTimesMs[SENSOR_BUFF] = {0};

    // Times in ms
    unsigned long startTimeMs = 0;
    unsigned long runningTimeMs = 0;
    unsigned long tripTimeMs = 0;

    // all in meters or meters/per hour
    float distance = 0.0;
    float speed = 0.0;
    float maxSpeed = 0.0;
    float avgSpeed = 0.0;
    // float odoDistance = 0.0; // total meters

    float circumference = Wheels[WheelNumber].circumference;

public:
    void init()
    {
        this->Memory = this->readMemory();
        // Reset Day counter?
        TimeElements time;
        breakTime( this->Memory.timestamp, time );
        if ( DEBUG ) {
            Serial.print("\tDay:\t");
            Serial.print(time.Day);
            Serial.print("\tToday:\t");
            Serial.print(day());
            Serial.println();
        }

        if ( time.Day != day() || time.Month != month() || time.Year != year() ) {
            this->Memory.prevDayDistance = this->Memory.dayDistance;
            this->Memory.prevDayAverage = this->Memory.dayAverageSpeed;
            this->Memory.prevDayMaxSpeed = this->Memory.dayMaxSpeed;
            this->Memory.dayDistance = 0.0;

        }
        // this->Memory.tripDistance = 0.0;
        // this->Memory.dayDistance = 0.0;
        // this->Memory.totalDistance += 200000.0;
    }

    bool isStarted()
    {
        return this->started;
    }

    bool isPaused()
    {
        return this->paused;
    }

    bool getSpeedSensor()
    {
        return this->SpeedSensor;
    }

    float getSpeed()
    {
        return this->speed;
    }

    float getAvgSpeed()
    {
        return this->avgSpeed;
    }

    bool isFaster()
    {
        return this->speed > this->avgSpeed;
    }

    float getMaxSpeed()
    {
        return this->maxSpeed;
    }

    float getDistance()
    {
        return this->distance / 1000;
    }

    float getDayDistance()
    {
        return this->Memory.dayDistance / 1000;
    }

    float getTripDistance()
    {
        return this->Memory.tripDistance / 1000;
    }

    unsigned long getTotalDistance()
    {
        return (int)this->Memory.totalDistance / 1000;
    }

    float getPrevDistance() {
        return this->Memory.prevDayDistance / 1000;
    }

    float getPrevAvgSpeed() {
        return this->Memory.prevDayAverage;
    }

    float getPrevMaxSpeed() {
        return this->Memory.prevDayMaxSpeed;
    }



    unsigned long getTripTime()
    {
        return this->tripTimeMs;
    }

    unsigned long getTotalTime()
    {
        return millis() - this->startTimeMs;
    }

    void loop()
    {
        unsigned long now = millis();

        if (TEST) {
            int value = analogRead(PIN_TEST_SPEED);
            this->speed = map( value,0,1023,0,100);
        }

        // Read & clear sensor buffers
        // & Calc sensor time average (only those that are larger than 0)
        // & Calc moved distance
        float movedDistance = 0.0;

        int buffLength = SENSOR_BUFF;
        unsigned long totalSensorTime = 0;
        for (size_t i = 0; i < SENSOR_BUFF; i++)
        {
            if (this->sensorTimesMs[i] > 0)
            {
                totalSensorTime += this->sensorTimesMs[i];
                this->sensorTimesMs[i] = 0;
                movedDistance += this->circumference;
            }
            else
            {
                buffLength--;
            }
        }
        if ( TEST ) {
            movedDistance = this->speed * 1000 / 60 / 60;
        }

        // Add moved to all distances
        this->distance += movedDistance;
        this->Memory.dayDistance += movedDistance;
        this->Memory.tripDistance += movedDistance;
        this->Memory.totalDistance += movedDistance;

        if (buffLength > 0 || (TEST && this->speed > 2) )
        {
            this->paused = false;
            if (this->pausedTime > 0)
            {
                this->tripTimeMs -= (now - this->pausedTime);
                this->pausedTime = 0;
            }
            this->SpeedSensor = !this->SpeedSensor;
            if (!this->started)
            {
                this->startTimeMs = now;
                this->runningTimeMs = now;
                this->started = true;
            }
        }


        if (this->started)
        {

            // Calc speed
            if ( !TEST ) {
                unsigned long avgSensorTime = 0;
                if (buffLength > 0 && totalSensorTime > 0)
                {
                    avgSensorTime = totalSensorTime / buffLength;
                    float Speed_meter_sec = this->circumference / (avgSensorTime / 1000.0);
                    this->speed = Speed_meter_sec * 3.6;
                }
            }

            // Calc others
            this->avgSpeed = this->distance / (this->tripTimeMs / 1000.0) * 3.6;
            if (this->speed < 10000000 && this->speed >= this->maxSpeed)
            {
                this->maxSpeed = this->speed;
            }
            this->tripTimeMs += now - this->runningTimeMs;
            this->runningTimeMs = now;

            if ( !TEST ) {
                if (buffLength == 0 && (now - this->lastSensorTimeMs > MAX_SENSOR_TIME))
                {
                    this->speed = 0.0;
                    if (!this->paused)
                    {
                        this->paused = true;
                        this->pausedTime = now;
                        this->storeMemory();
                    }
                }
            }

        }

        if ( DEBUG ) {
            Serial.print("\tStarted:\t");
            Serial.print(this->started);
            Serial.print("\tPaused:\t");
            Serial.print(this->paused);
            Serial.print("\tSpeed:\t");
            Serial.print(this->speed);

            Serial.print("\tDist:\t");
            Serial.print(this->distance);
            Serial.print("\tDay:\t");
            Serial.print(this->Memory.dayDistance);
            Serial.print("\tTrip:\t");
            Serial.print(this->Memory.tripDistance);
            Serial.print("\tTotal:\t");
            Serial.print(this->Memory.totalDistance);
            Serial.println();
        }

        // Only store ODO if it has changed, so EEPROM is only written when needed
        unsigned long oldOdo = (int)this->readMemory().totalDistance;
        unsigned long currentOdo = (int)this->Memory.totalDistance;
        if (currentOdo != oldOdo)
        {
            this->storeMemory();
        }
    }

    void storeMemory()
    {
        this->Memory.timestamp = now();
        this->Memory.dayTimeMovedSecs = this->runningTimeMs / 1000;
        this->Memory.dayAverageSpeed = this->avgSpeed;
        this->Memory.dayMaxSpeed = this->maxSpeed;
        this->Memory.wheelCircumference = this->circumference;
        EEPROM.put(MEM_ADDRESS, this->Memory);
        EEPROM.put(ODO_ADDRESS, this->Memory.totalDistance);
    }

    MemoryStruct readMemory()
    {
        // float readOdo = 0;
        // EEPROM.get(ODO_ADDRESS, readOdo);
        MemoryStruct tmpMemory;
        EEPROM.get(MEM_ADDRESS, tmpMemory);
        return tmpMemory;
    }

    void sensorTrigger()
    {
        unsigned long now = millis();
        unsigned long sensorTime = now - this->lastSensorTimeMs;
        if (sensorTime > MIN_SENSOR_TIME)
        {
            this->lastSensorTimeMs = now;
            // Shift sensor times
            for (size_t i = 0; i < SENSOR_BUFF - 1; i++)
            {
                this->sensorTimesMs[i] = this->sensorTimesMs[i + 1];
            }
            // Add new sensor time
            this->sensorTimesMs[SENSOR_BUFF - 1] = sensorTime;
        }
    }
};
