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

public:
    void init()
    {
        this->Memory = this->readMemory();

        // if ( DEBUG ) {
        //     Serial.print("\tDay:\t");
        //     Serial.print(time.Day);
        //     Serial.print("\tMonth:\t");
        //     Serial.print(time.Month);
        //     // Serial.print("\tYear:\t");
        //     // Serial.print(time.Year);
        //     Serial.print("\t\tToday:\t");
        //     Serial.print(day());
        //     Serial.print("\tMonth:\t");
        //     Serial.print(month());
        //     // Serial.print("\tYear:\t");
        //     // Serial.print(year());
        //     Serial.println();
        // }
    }

    void resetDay() {
        this->Memory.prevDayDistance = this->Memory.dayDistance;
        this->Memory.prevDayAverage = this->Memory.dayAverageSpeed;
        this->Memory.prevDayMaxSpeed = this->Memory.dayMaxSpeed;
        this->Memory.dayDistance = 0.0;
    }

    void resetDistance() {
        this->distance = 0.0;
        this->started = false;
        this->paused = true;
    }

    void continueDay() {
        this->started = true;
        this->paused = true;
        this->tripTimeMs = this->Memory.dayTimeMovedSecs * 1000;
        this->distance = this->Memory.dayDistance;
        this->avgSpeed = this->Memory.dayAverageSpeed;
        this->maxSpeed = this->Memory.dayMaxSpeed;
    }

    bool IsNewDay() {
        TimeElements time;
        breakTime( this->Memory.timestamp, time );
        return ( time.Day != day() || time.Month != month() );
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

    void setWheelCircumference(float circumference) {
        this->Memory.wheelCircumference = circumference;
    }

    float getWheelCircumference() {
        return this->Memory.wheelCircumference;
    }

    unsigned int getClosestETRTO() {
        int closest = 0;
        for (size_t i = 1; i < NR_ETRTO_WHEELS; i++)
        {
            float diffLower = Wheels[i-1].circumference - this->Memory.wheelCircumference;
            float diffHigher = Wheels[i].circumference - this->Memory.wheelCircumference;
            if (diffLower <= 0 && diffHigher >= 0) {
                if ( -diffLower < diffHigher ) {
                    closest = i-1;
                }
                else {
                    closest = i;
                }
            }
        }
        return closest;
    }

    void increaseCircumference( int inc = 1 ) {
        if ( inc > 1000 || inc < -1000 ) {
            int wheelnr =  this->getClosestETRTO();
            if ( inc > 1000 ) {
                wheelnr++;
            }
            if ( inc < -1000 ) {
                wheelnr--;
            }
            if ( wheelnr > NR_ETRTO_WHEELS ) {
                wheelnr = NR_ETRTO_WHEELS;
            }
            if ( wheelnr < 0 ) {
                wheelnr = 0;
            }
            this->Memory.wheelCircumference = Wheels[wheelnr].circumference;
        }
        else {
            this->Memory.wheelCircumference += (inc / 1000.0);
        }
    }

    void decreaseCircumference(int inc = 1) {
        this->increaseCircumference( - inc );
    }

    void increaseClock( int inc = 1 ) {
        if (inc == 1) {
            setTime( hour(), minute() + 1,0,day(),month(),year());
        }
        if (inc == 10) {
            setTime( hour(), minute() + 10,0,day(),month(),year());
        }
        if (inc == 1000) {
            setTime( hour()+1, minute(),0,day(),month(),year());
        }
    }

    void decreaseClock(int inc = 1) {
        if (inc == 1 && minute() > 0) {
            setTime( hour(), minute() - 1,0,day(),month(),year());
        }
        if (inc == 10 && minute() > 10 ) {
            setTime( hour(), minute() - 10,0,day(),month(),year());
        }
        if (inc == 1000 && hour() > 0) {
            setTime( hour()-1, minute(),0,day(),month(),year());
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

    void resetTripDistance() {
        this->Memory.tripDistance = 0;
    }

    unsigned long getTotalDistance()
    {
        return (int)this->Memory.totalDistance / 1000;
    }

    void increaseTotal( int inc = 1 ) {
        this->Memory.totalDistance += (inc * 1000.0);
    }

    void decreaseTotal(int inc = 1) {
        this->increaseTotal( - inc );
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
            // Dummy speed
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
                movedDistance += this->Memory.wheelCircumference;
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
        if ( movedDistance > 0 && !this->started )
        {
            this->started = true;
            this->paused = false;
            this->startTimeMs = now;
            this->runningTimeMs = now;
        }
        this->distance += movedDistance;
        this->Memory.dayDistance += movedDistance;
        this->Memory.tripDistance += movedDistance;
        this->Memory.totalDistance += movedDistance;

        if ( this->started )
        {

            // Calc speed
            if ( !TEST ) {
                unsigned long avgSensorTime = 0;
                this->speed = 0.0;
                if (buffLength > 0 && totalSensorTime > 0)
                {
                    avgSensorTime = totalSensorTime / buffLength;
                    float Speed_meter_sec = this->Memory.wheelCircumference / (avgSensorTime / 1000.0);
                    this->speed = Speed_meter_sec * 3.6;
                }
            }

            // if ( DEBUG ) {
            //     Serial.print("Speed:\t");
            //     Serial.print(this->speed);
            //     Serial.print("\tBuffer");
            //     Serial.print(buffLength);
            //     Serial.println();
            // }

            // Calc AVG & MAX
            this->avgSpeed = this->distance / (this->tripTimeMs / 1000.0) * 3.6;
            if (this->speed < 10000000 && this->speed >= this->maxSpeed)
            {
                this->maxSpeed = this->speed;
            }

            // Paused??
            if ( this->speed <= 2.0 )
            {
                if ( !this->paused )
                {
                    this->paused = true;
                    this->storeMemory();
                }
            }

        }

        // Pause & TrimTime & SpeedSensor
        if ( this->speed > 2.0 )
        {
            if ( this->paused ) {
                this->paused = false;
                this->runningTimeMs = now;
            }
            this->tripTimeMs += (now - this->runningTimeMs);
            this->runningTimeMs = now;
            this->SpeedSensor = !this->SpeedSensor;
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
        this->Memory.dayTimeMovedSecs = this->tripTimeMs / 1000;
        this->Memory.dayAverageSpeed = this->avgSpeed;
        this->Memory.dayMaxSpeed = this->maxSpeed;
        EEPROM.put(MEM_ADDRESS, this->Memory);
        EEPROM.put(ODO_ADDRESS, this->Memory.totalDistance);
    }

    MemoryStruct readMemory()
    {
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
