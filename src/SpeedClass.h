#pragma once

#include "Arduino.h"
#include <EEPROM.h>

#define PAUSE_THRESHOLD         1.0
#define SPEED_FILTER_THRESHOLD  15.0
#define SPEED_FILTER_RATIO      2.0

// #define MEM_ADDRESS 4
#define NEW_ADDRESS 50

class SpeedClass
{

private:
    bool started = false;
    bool paused = true;

    // Speed sensor
    bool SpeedSensor = true;
    volatile unsigned long lastSensorTimeMs = 0;
    volatile unsigned long sensorTimesMs[SENSOR_BUFF] = {0};

    MemoryStruct Memory;

    unsigned long tempTimeMs = 0;
    float speed = 0.0;

public:
    void init()
    {
        this->Memory = this->readMemory();
        if ( DEBUG ) {
            // this->_printMemory("READ");
        }
        this->started = false;
        this->paused = true;
    }

    void start()
    {
        this->started = true;
        this->paused = false;
    }

    void continueCurrent()
    {
        this->tempTimeMs = millis();
    }

    void resetCurrent()
    {
        this->tempTimeMs = millis();
        this->Memory.currentDistance = 0.0;
        this->Memory.currentStartTime = now();
        this->Memory.currentTime = 0.0;
        this->Memory.currentAverageSpeed = 0.0;
        this->Memory.currentMaxSpeed = 0.0;
    }

    void pauseCurrent()
    {
        if (!this->paused)
        {
            this->paused = true;
            this->storeMemory();
        }
    }

    void unPauseCurrent() {
        this->paused = false;
        this->tempTimeMs = millis();
    }

    void startDay()
    {
        this->Memory.prevDistance = this->Memory.dayDistance;
        this->Memory.prevTime = this->Memory.dayTime;
        this->Memory.prevAverageSpeed = this->Memory.dayAverageSpeed;
        this->Memory.prevMaxSpeed = this->Memory.dayMaxSpeed;
        this->Memory.dayDistance = 0.0;
        this->Memory.dayTime = 0.0;
        this->Memory.dayAverageSpeed = 0.0;
        this->Memory.dayMaxSpeed = 0.0;
        this->resetCurrent();
    }

    bool IsNewDay()
    {
        TimeElements time;
        breakTime(this->Memory.timestamp, time);
        return (time.Day != day() || time.Month != month());
    }

    bool IsShortPause() {
        return (now() - this->Memory.timestamp) < MAX_TIME_SHORT_BRAKE;
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

    void setWheelCircumference(float circumference)
    {
        this->Memory.wheelCircumference = circumference;
    }

    float getWheelCircumference()
    {
        return this->Memory.wheelCircumference;
    }

    unsigned int getClosestETRTO()
    {
        int closest = 0;
        for (size_t i = 1; i < NR_ETRTO_WHEELS; i++)
        {
            float diffLower = Wheels[i - 1].circumference - this->Memory.wheelCircumference;
            float diffHigher = Wheels[i].circumference - this->Memory.wheelCircumference;
            if (diffLower <= 0 && diffHigher >= 0)
            {
                if (-diffLower < diffHigher)
                {
                    closest = i - 1;
                }
                else
                {
                    closest = i;
                }
            }
        }
        return closest;
    }

    void increaseCircumference(int inc = 1)
    {
        if (inc > 1000 || inc < -1000)
        {
            int wheelnr = this->getClosestETRTO();
            if (inc > 1000)
            {
                wheelnr++;
            }
            if (inc < -1000)
            {
                wheelnr--;
            }
            if (wheelnr > NR_ETRTO_WHEELS)
            {
                wheelnr = NR_ETRTO_WHEELS;
            }
            if (wheelnr < 0)
            {
                wheelnr = 0;
            }
            this->Memory.wheelCircumference = Wheels[wheelnr].circumference;
        }
        else
        {
            this->Memory.wheelCircumference += (inc / 1000.0);
        }
    }

    void decreaseCircumference(int inc = 1)
    {
        this->increaseCircumference(-inc);
    }

    void increaseClock(int inc = 1)
    {
        if (inc == 1)
        {
            setTime(hour(), minute() + 1, 0, day(), month(), year());
        }
        if (inc == 10)
        {
            setTime(hour(), minute() + 10, 0, day(), month(), year());
        }
        if (inc == 1000)
        {
            setTime(hour() + 1, minute(), 0, day(), month(), year());
        }
    }

    void decreaseClock(int inc = 1)
    {
        if (inc == 1 && minute() > 0)
        {
            setTime(hour(), minute() - 1, 0, day(), month(), year());
        }
        if (inc == 10 && minute() > 10)
        {
            setTime(hour(), minute() - 10, 0, day(), month(), year());
        }
        if (inc == 1000 && hour() > 0)
        {
            setTime(hour() - 1, minute(), 0, day(), month(), year());
        }
    }

    float getSpeed()
    {
        return this->speed;
    }

    float getAvgSpeed()
    {
        return this->Memory.currentAverageSpeed;
    }

    int speedDiffScale() {
        float diff = this->speed - this->Memory.currentAverageSpeed;
        int iDiff = abs(map(diff,-30,30,-9,9));
        if (iDiff>9) iDiff = 9;
        return abs(iDiff);
    }

    bool isFaster()
    {
        return this->speed > this->Memory.currentAverageSpeed;
    }

    float getCurrentMaxSpeed()
    {
        return this->Memory.currentMaxSpeed;
    }

    float getCurrentDistance()
    {
        return this->Memory.currentDistance / 1000;
    }

    float getDayDistance()
    {
        return this->Memory.dayDistance / 1000;
    }

    // int getLengthDistance() {
    //     return int(this->Memory.currentDistance*100) % BIKE_LENGTH;
    // }

    float getTripDistance(int trip = 1)
    {
        float dist = 0;
        switch (trip)
        {
        case 1:
            dist = this->Memory.tripDistance1;
            break;
        case 2:
            dist = this->Memory.tripDistance2;
            break;
        case 3:
            dist = this->Memory.tripDistance3;
            break;
        }
        return dist / 1000;
    }

    void resetTripDistance(int trip = 1)
    {
        switch (trip)
        {
        case 1:
            this->Memory.tripDistance1 = 0;
            break;
        case 2:
            this->Memory.tripDistance2 = 0;
            break;
        case 3:
            this->Memory.tripDistance3 = 0;
            break;
        }
    }

    unsigned long getTotalDistance()
    {
        return (int)this->Memory.totalDistance / 1000;
    }

    void increaseTotal(int inc = 1)
    {
        this->Memory.totalDistance += (inc * 1000.0);
    }

    void decreaseTotal(int inc = 1)
    {
        this->increaseTotal(-inc);
    }

    float getPrevDistance()
    {
        return this->Memory.prevDistance / 1000;
    }

    float getPrevAvgSpeed()
    {
        return this->Memory.prevAverageSpeed;
    }

    float getPrevMaxSpeed()
    {
        return this->Memory.prevMaxSpeed;
    }

    unsigned long getCurrentTime()
    {
        return this->Memory.currentTime;
    }

    unsigned long getTotalTime()
    {
        return (now() - this->Memory.currentStartTime) * 1000;
    }

    void loop()
    {
        unsigned long now = millis();

        if (TEST)
        {
            // Dummy speed (random)
            unsigned long sensorTime = random(0,500);
            for (size_t i = 0; i < SENSOR_BUFF; i++)
            {
                this->sensorTimesMs[i] = sensorTime;
            }
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
                movedDistance += this->Memory.wheelCircumference;
            }
            else
            {
                buffLength--;
            }
        }

        // if (DEBUG)
        // {
        //     Serial.print("MOVED:");
        //     Serial.println(movedDistance);
        // }


        // start
        if (movedDistance > 0 && !this->started)
        {
            this->start();
        }

        if (!this->started)
        {
            return;
        }

        // if (DEBUG) {
        //     Serial.print(movedDistance);
        //     Serial.print("\t");
        // }

        // Add moved distance to all distances
        this->Memory.currentDistance += movedDistance;
        this->Memory.dayDistance += movedDistance;
        this->Memory.tripDistance1 += movedDistance;
        this->Memory.tripDistance2 += movedDistance;
        this->Memory.tripDistance3 += movedDistance;
        this->Memory.totalDistance += (int) movedDistance;

        // Calc speeds
        float lastSpeed = this->speed;
        this->speed = 0.0;
        unsigned long avgSensorTime = 0;
        if (buffLength > 0 && totalSensorTime > 0)
        {
            avgSensorTime = totalSensorTime / buffLength;
            float Speed_meter_sec = this->Memory.wheelCircumference / (avgSensorTime / 1000.0);
            this->speed = Speed_meter_sec * 3.6;
        }
        // Filter strange change in speed (possibly due to double trigger of sensor)
        if (this->speed >= SPEED_FILTER_THRESHOLD) {
            if (this->speed >= SPEED_FILTER_RATIO * lastSpeed) {
                this->speed = this->speed / SPEED_FILTER_RATIO;
            }
        }

        // Calc AVG & MAX
        if (this->Memory.currentDistance > 0.1 && this->Memory.currentTime > 1000) {
            this->Memory.currentAverageSpeed = this->Memory.currentDistance / (this->Memory.currentTime / 1000.0) * 3.6;
            if (this->speed < 120.0 && this->speed >= this->Memory.currentMaxSpeed)
            {
                this->Memory.currentMaxSpeed = this->speed;
            }
            if (this->Memory.currentAverageSpeed > this->Memory.currentMaxSpeed) {
                this->Memory.currentAverageSpeed = this->Memory.currentMaxSpeed;
            }
        }

        // Pause??
        if (this->speed <= PAUSE_THRESHOLD)
        {
            this->pauseCurrent();
        }

        if (this->speed > PAUSE_THRESHOLD)
        {
            if (this->paused)
            {
                this->unPauseCurrent();
            }
            this->Memory.currentTime += (now - this->tempTimeMs);
            this->tempTimeMs = now;
            this->SpeedSensor = !this->SpeedSensor;
        }

        // Only store ODO if it has changed (per 100m), so EEPROM is only written when needed
        unsigned long oldOdo = (int)this->readMemory().totalDistance / 100;
        unsigned long currentOdo = (int)this->Memory.totalDistance / 100;
        if (currentOdo != oldOdo)
        {
            this->storeMemory();
        }

        if ( DEBUG ) {
            // this->_printMemory("LOOP");
        }

    }

    void storeMemory()
    {
        this->Memory.timestamp = now();
        EEPROM.put(NEW_ADDRESS, this->Memory);
    }

    MemoryStruct readMemory()
    {
        MemoryStruct tmpMemory;
        EEPROM.get(NEW_ADDRESS, tmpMemory);
        return tmpMemory;
    }

    void _printMemory(const char message[]) {
        Serial.print(message);
        Serial.print("\t");
        Serial.print("now: ");Serial.print(now());Serial.print("      \t");
        Serial.print("stamp: ");Serial.print(this->Memory.timestamp);Serial.print("      \t");
        // Serial.print("diff (sec): ");Serial.print(now() - this->Memory.timestamp);Serial.print("      \t");
        // Serial.print("totalDistance: ");Serial.print(this->Memory.totalDistance);Serial.print("      \t");
        // Serial.print("tripDistance: ");Serial.print(this->Memory.tripDistance/ 1000);Serial.print("      \t");
        // Serial.print("dayDistance: ");Serial.print(this->Memory.dayDistance/1000);Serial.print("      \t");
        // Serial.print("dayTime: ");Serial.print(this->Memory.dayTime);Serial.print("      \t");
        // Serial.print("dayAverageSpeed: ");Serial.print(this->Memory.dayAverageSpeed);Serial.print("      \t");

        // Serial.print("Speed: ");Serial.print(this->speed);Serial.print("      \t");
        // Serial.print("currentDistance: ");Serial.print(this->Memory.currentDistance/1000);Serial.print("      \t");
        Serial.print("currentTime: ");Serial.print(this->Memory.currentTime);Serial.print("      \t");
        Serial.print("totalTime: ");Serial.print(this->getTotalTime());Serial.print("      \t");

        // Serial.print("CurrentAvgSpeed: ");Serial.print(this->Memory.currentAverageSpeed);Serial.print("      \t");
        // Serial.print("CurrentMaxSpeed: ");Serial.print(this->Memory.currentMaxSpeed);Serial.print("      \t");

        // Serial.print("dayMaxSpeed: ");Serial.print(this->Memory.dayMaxSpeed);Serial.print("      \t");
        // Serial.print("prevDistance: ");Serial.print(this->Memory.prevDistance);Serial.print("      \t");
        // Serial.print("prevAverageSpeed: ");Serial.print(this->Memory.prevAverageSpeed);Serial.print("      \t");
        // Serial.print("prevMaxSpeed: ");Serial.print(this->Memory.prevMaxSpeed);Serial.print("      \t");

        // Serial.print("wheelCircumference: ");Serial.print(this->Memory.wheelCircumference);Serial.print("      \t");

        Serial.print("\r");
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
