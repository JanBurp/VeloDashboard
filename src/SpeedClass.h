#pragma once

#include "Arduino.h"
#include <EEPROM.h>

#define ODO_ADDRESS 0

class SpeedClass
{

private:
    bool started = false;
    bool paused = true;
    unsigned long pausedTime = 0;

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
    float odoDistance = 0.0; // total meters

    float circumference = Wheels[WheelNumber].circumference;

public:
    void init()
    {
        this->odoDistance = this->readODO();
        // this->odoDistance = 2420000.0;
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

    unsigned long getOdoDistance()
    {
        return (int)this->odoDistance / 1000;
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

        // Read & clear sensor buffers
        // & Calc sensor time average (only those that are larger than 0)
        int buffLength = SENSOR_BUFF;
        unsigned long totalSensorTime = 0;
        for (size_t i = 0; i < SENSOR_BUFF; i++)
        {
            if (this->sensorTimesMs[i] > 0)
            {
                totalSensorTime += this->sensorTimesMs[i];
                this->sensorTimesMs[i] = 0;
                this->distance += this->circumference;
                this->odoDistance += this->circumference;
            }
            else
            {
                buffLength--;
            }
        }

        if (buffLength > 0)
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
            unsigned long avgSensorTime = 0;
            if (buffLength > 0 && totalSensorTime > 0)
            {
                // Calc speeds & times
                avgSensorTime = totalSensorTime / buffLength;
                float Speed_meter_sec = this->circumference / (avgSensorTime / 1000.0);
                this->speed = Speed_meter_sec * 3.6;
                // Avg speed
                this->avgSpeed = this->distance / (this->tripTimeMs / 1000.0) * 3.6;
                // Max speed
                if (this->speed < 10000000 && this->speed >= this->maxSpeed)
                {
                    this->maxSpeed = this->speed;
                }
                // Triptimes
                this->tripTimeMs += now - this->runningTimeMs;
                this->runningTimeMs = now;
            }

            if (buffLength == 0 && (now - this->lastSensorTimeMs > MAX_SENSOR_TIME))
            {
                this->speed = 0.0;
                if (!this->paused)
                {
                    this->paused = true;
                    this->pausedTime = now;
                    this->storeODO();
                }
            }
        }

        // Only store ODO if it has changed, so EEPROM is only written when needed
        unsigned long oldOdo = (int)this->readODO();
        unsigned long currentOdo = (int)this->odoDistance;
        if (currentOdo != oldOdo)
        {
            this->storeODO();
        }
    }

    void storeODO()
    {
        EEPROM.put(ODO_ADDRESS, this->odoDistance);
    }

    float readODO()
    {
        float readOdo = 0;
        EEPROM.get(ODO_ADDRESS, readOdo);
        return readOdo;
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
