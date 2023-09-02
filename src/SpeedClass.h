#pragma once

#include "Arduino.h"
#include <EEPROM.h>

#define PAUSE_THRESHOLD 1.0

#define MEM_ADDRESS 4
#define NEW_ADDRESS 50

class SpeedClass
{

private:
    bool started = false;
    bool paused = true;

    MemoryStruct Memory;
    newMemoryStruct newMemory;

    // Speed sensor
    bool SpeedSensor = true;
    volatile unsigned long lastSensorTimeMs = 0;
    volatile unsigned long sensorTimesMs[SENSOR_BUFF] = {0};

    // Times in ms
    unsigned long startTimeMs = 0;
    unsigned long tempTimeMs = 0;
    unsigned long timeMs = 0;

    // all in meters or meters/per hour
    float distance = 0.0;
    float speed = 0.0;
    float maxSpeed = 0.0;
    float avgSpeed = 0.0;

public:
    void init()
    {
        this->Memory = this->readMemory();
        if ( DEBUG ) {
            this->_printMemory("READ");
        }

    }

    void startCounter()
    {
        this->started = true;
        this->paused = false;
        this->startTimeMs = millis();
        this->tempTimeMs = millis();
    }

    void pauseCounter()
    {
        if (!this->paused)
        {
            this->paused = true;
            this->storeMemory();
        }
    }

    void continueCounter()
    {
        this->paused = false;
        this->tempTimeMs = millis();
    }


    void resetDay()
    {
        this->Memory.prevDayDistance = this->Memory.dayDistance;
        this->Memory.prevDayAverage = this->Memory.dayAverageSpeed;
        this->Memory.prevDayMaxSpeed = this->Memory.dayMaxSpeed;
        this->Memory.dayDistance = 0.0;
    }

    void resetDistance()
    {
        this->distance = 0.0;
        this->timeMs = 0.0;
        this->started = false;
        this->paused = true;
    }

    void continueDay()
    {
        this->started = true;
        this->paused = true;
        this->timeMs = this->Memory.dayTimeMovedSecs * 1000.0;
        this->distance = this->Memory.dayDistance;
        this->avgSpeed = this->Memory.dayAverageSpeed;
        this->maxSpeed = this->Memory.dayMaxSpeed;
    }

    bool IsNewDay()
    {
        TimeElements time;
        breakTime(this->Memory.timestamp, time);
        return (time.Day != day() || time.Month != month());
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
        return this->avgSpeed;
    }

    int speedDiffScale() {
        float diff = this->speed - this->avgSpeed;
        int iDiff = abs(map(diff,-30,30,-9,9));
        if (iDiff>9) iDiff = 9;
        return abs(iDiff);
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

    void resetTripDistance()
    {
        this->Memory.tripDistance = 0;
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
        return this->Memory.prevDayDistance / 1000;
    }

    float getPrevAvgSpeed()
    {
        return this->Memory.prevDayAverage;
    }

    float getPrevMaxSpeed()
    {
        return this->Memory.prevDayMaxSpeed;
    }

    unsigned long getTripTime()
    {
        return this->timeMs;
    }

    unsigned long getTotalTime()
    {
        return millis() - this->startTimeMs;
    }

    void loop()
    {
        unsigned long now = millis();

        if (TEST)
        {
            // Dummy speed
            int value = analogRead(PIN_TEST_SPEED);
            unsigned long sensorTime = map(value,0,1023,10000, 1);
            // this->speed = map(value, 0, 1023, 0, 100);
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

        // start
        if (movedDistance > 0 && !this->started)
        {
            this->startCounter();
        }

        if (!this->started)
        {
            return;
        }

        // Add moved distance to all distances
        this->distance += movedDistance;
        this->Memory.dayDistance += movedDistance;
        this->Memory.tripDistance += movedDistance;
        this->Memory.totalDistance += movedDistance;

        // Calc speeds
        unsigned long avgSensorTime = 0;
        this->speed = 0.0;
        if (buffLength > 0 && totalSensorTime > 0)
        {
            avgSensorTime = totalSensorTime / buffLength;
            float Speed_meter_sec = this->Memory.wheelCircumference / (avgSensorTime / 1000.0);
            this->speed = Speed_meter_sec * 3.6;
        }

        // Calc AVG & MAX
        this->avgSpeed = this->distance / (this->timeMs / 1000.0) * 3.6;
        if (this->speed < 120.0 && this->speed >= this->maxSpeed)
        {
            this->maxSpeed = this->speed;
        }
        if (this->avgSpeed > this->maxSpeed) {
            this->avgSpeed = this->maxSpeed;
        }

        // Pause??
        if (this->speed <= PAUSE_THRESHOLD)
        {
            this->pauseCounter();
        }

        if (this->speed > PAUSE_THRESHOLD)
        {
            if (this->paused)
            {
                this->continueCounter();
            }
            this->timeMs += (now - this->tempTimeMs);
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
    }

    void storeMemory()
    {
        this->Memory.timestamp = now();
        this->Memory.dayTimeMovedSecs = this->timeMs / 1000;
        this->Memory.dayAverageSpeed = this->avgSpeed;
        this->Memory.dayMaxSpeed = this->maxSpeed;
        EEPROM.put(MEM_ADDRESS, this->Memory);

        // new struct
        this->newMemory.timestamp           = this->Memory.timestamp;              // Time when data is stored
        this->newMemory.totalDistance       = this->Memory.totalDistance;          // Total ODO
        this->newMemory.tripDistance1       = this->Memory.tripDistance;           // trip totals 1..3
        this->newMemory.tripDistance2       = 0;
        this->newMemory.tripDistance3       = 0;
        this->newMemory.currentDistance     = this->distance;                      // current
        this->newMemory.currentTimeSecs     = this->timeMs;
        this->newMemory.currentAverageSpeed = this->avgSpeed;
        this->newMemory.currentMaxSpeed     = this->maxSpeed;
        this->newMemory.dayDistance         = this->Memory.dayDistance;            // day totals
        this->newMemory.dayTimeSecs         = this->Memory.dayTimeMovedSecs;
        this->newMemory.dayAverageSpeed     = this->Memory.dayAverageSpeed;
        this->newMemory.dayMaxSpeed         = this->Memory.dayMaxSpeed;
        this->newMemory.prevDayDistance     = this->Memory.prevDayDistance;        // prev day
        this->newMemory.prevDayAverage      = this->Memory.prevDayAverage;
        this->newMemory.prevDayMaxSpeed     = this->Memory.prevDayMaxSpeed;
        this->newMemory.wheelCircumference  = this->Memory.wheelCircumference;     // config
        EEPROM.put(NEW_ADDRESS, this->newMemory);

        if ( DEBUG ) {
            this->_printMemory("STORE");
        }
    }

    MemoryStruct readMemory()
    {
        MemoryStruct tmpMemory;
        EEPROM.get(MEM_ADDRESS, tmpMemory);
        return tmpMemory;
    }

    void _printMemory(const char message[]) {
        Serial.print(message);
        Serial.print("\t");
        // Serial.print("stamp: ");Serial.print(this->Memory.timestamp);Serial.print("\t");
        Serial.print("totalDistance: ");Serial.print(this->Memory.totalDistance);Serial.print("\t");
        // Serial.print("tripDistance: ");Serial.print(this->Memory.tripDistance/ 1000);Serial.print("\t");
        Serial.print("dayDistance: ");Serial.print(this->Memory.dayDistance/1000);Serial.print("\t");
        Serial.print("dayTimeMovedSecs: ");Serial.print(this->Memory.dayTimeMovedSecs);Serial.print("\t");
        Serial.print("dayAverageSpeed: ");Serial.print(this->Memory.dayAverageSpeed);Serial.print("\t");

        Serial.print("Speed: ");Serial.print(this->speed);Serial.print("\t");
        Serial.print("distance: ");Serial.print(this->distance/1000);Serial.print("\t");
        Serial.print("tripTime: ");Serial.print(this->timeMs);Serial.print("\t");
        Serial.print("avgSpeed: ");Serial.print(this->avgSpeed);Serial.print("\t");
        Serial.print("maxSpeed: ");Serial.print(this->maxSpeed);Serial.print("\t");



        // Serial.print("dayMaxSpeed: ");Serial.print(this->Memory.dayMaxSpeed);Serial.print("\t");
        // Serial.print("prevDayDistance: ");Serial.print(this->Memory.prevDayDistance);Serial.print("\t");
        // Serial.print("prevDayAverage: ");Serial.print(this->Memory.prevDayAverage);Serial.print("\t");
        // Serial.print("prevDayMaxSpeed: ");Serial.print(this->Memory.prevDayMaxSpeed);Serial.print("\t");
        Serial.println();
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
