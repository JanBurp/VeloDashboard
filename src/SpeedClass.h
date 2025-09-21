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

    // Cadans sensor
    volatile unsigned long lastCadansTimeMs = 0;
    volatile unsigned long cadansTimesMs[SENSOR_BUFF] = {0};
    unsigned int cadans = 0;

    MemoryStruct Memory;

    unsigned long tempTimeMs = 0;
    float speed = 0.0;

public:
    void init()
    {
        this->Memory = this->readMemory();
        if ( DEBUG ) {
            this->_printMemory("READ");
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
        if (this->Memory.dayDistance == 0) {
            this->Memory.dayStartTime = now();
        }
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
        this->Memory.dayStartTime = now();
        this->Memory.dayAverageSpeed = 0.0;
        this->Memory.dayMaxSpeed = 0.0;
        this->resetCurrent();
        // if ( DEBUG ) {
        //     this->_printMemory("STARTDAY");
        // }
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

    int getCadans()
    {
        return this->cadans;
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

    unsigned long getCurrentTime()
    {
        return this->Memory.currentTime;
    }

    unsigned long getTotalTime()
    {
        return (now() - this->Memory.currentStartTime) * 1000;
    }

    float getDayDistance()
    {
        return this->Memory.dayDistance / 1000;
    }

    float getDayAvgSpeed() {
        return this->Memory.dayAverageSpeed;
    }

    float getDayMaxSpeed() {
        return this->Memory.dayMaxSpeed;
    }

    unsigned long getDayTime()
    {
        return this->Memory.dayTime;
    }

    unsigned long getDayTotalTime()
    {
        return (now() - this->Memory.dayStartTime) * 1000;
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

    unsigned long getprevTime()
    {
        return this->Memory.prevTime;
    }


    void loop()
    {
        unsigned long now = millis();

        if (TEST)
        {
            // Dummy speed
            unsigned long sensorTime = (cos(millis()/5000) + 1) * 600;
            for (size_t i = 0; i < SENSOR_BUFF; i++)
            {
                this->sensorTimesMs[i] = sensorTime;
            }
            // Dummy cadans
            sensorTime = (cos(millis()/5000) + 1) * 400 + 300;
            for (size_t i = 0; i < SENSOR_BUFF; i++)
            {
                this->cadansTimesMs[i] = sensorTime;
            }
        }

        // SPEED
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

        // CADANS (test)
        int cadansBuffLength = SENSOR_BUFF;
        unsigned long totalCadansTime = 0;
        unsigned long cadansTriggers = 0;
        for (size_t i = 0; i < SENSOR_BUFF; i++)
        {
            if (this->cadansTimesMs[i] > 0)
            {
                totalCadansTime += this->cadansTimesMs[i];
                cadansTriggers++;
                this->cadansTimesMs[i] = 0;
            }
            else
            {
                cadansBuffLength--;
            }
        }
        this->cadans = 0;
        float avgCadansTime = 0;
        if (cadansBuffLength > 0 && totalCadansTime > 0) {
            avgCadansTime = totalCadansTime / cadansBuffLength;
            this->cadans = int(float(SPEED_CALCULATION_TIME) / avgCadansTime * float(MINUTE/SPEED_CALCULATION_TIME));
        }
        // if (DEBUG)
        // {
        //     Serial.print("avgCadansTime:");
        //     Serial.print(avgCadansTime);
        //     Serial.print("\tCADANS:\t");
        //     Serial.println(this->cadans);
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
            this->Memory.dayAverageSpeed = this->Memory.dayDistance / (this->Memory.dayTime / 1000.0) * 3.6;
            // this->Memory.currentAverageSpeed = this->Memory.currentDistance / (float(this->Memory.currentTime)/1000.0) * 3.6;
            // this->Memory.dayAverageSpeed = this->Memory.dayDistance / (float(this->Memory.dayTime)/1000.0) * 3.6;
            if (this->speed < 120.0)
            {
                if (this->speed >= this->Memory.currentMaxSpeed) {
                    this->Memory.currentMaxSpeed = this->speed;
                }
                if (this->speed >= this->Memory.dayMaxSpeed) {
                    this->Memory.dayMaxSpeed = this->speed;
                }
            }
            if (this->Memory.currentAverageSpeed > this->Memory.currentMaxSpeed) {
                this->Memory.currentAverageSpeed = this->Memory.currentMaxSpeed;
            }
            if (this->Memory.dayAverageSpeed > this->Memory.dayMaxSpeed) {
                this->Memory.dayAverageSpeed = this->Memory.dayMaxSpeed;
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
            this->Memory.dayTime += (now - this->tempTimeMs);
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

        // if ( DEBUG ) {
        //     this->_printMemory("LOOP");
        // }

    }

    void storeMemory()
    {
        if ( ! TEST) {
            this->Memory.timestamp = now();
            EEPROM.put(NEW_ADDRESS, this->Memory);
            if ( DEBUG ) {
                this->_printMemory("STORE");
            }
        }

    }

    MemoryStruct readMemory()
    {
        MemoryStruct tmpMemory;
        EEPROM.get(NEW_ADDRESS, tmpMemory);
        return tmpMemory;
    }

    void _printMemory(const char message[]) {
        Serial.println();
        Serial.println(message);
        // Serial.print("now: ");Serial.print(now());
        // Serial.print("stamp: ");Serial.print(this->Memory.timestamp);
        // Serial.print("diff (sec): ");Serial.print(now() - this->Memory.timestamp);

        // Serial.print("Speed: ");Serial.print(this->speed);
        Serial.print("\tCURR:");
        Serial.print("\tDst:");Serial.print(this->Memory.currentDistance/1000);
        Serial.print("\tAvg:");Serial.print(this->Memory.currentAverageSpeed);
        Serial.print("\tMax:");Serial.print(this->Memory.currentMaxSpeed);
        Serial.print("\tTime:");Serial.print(this->Memory.currentTime);
        Serial.print("\tTotal:");Serial.print(this->getTotalTime());
        Serial.print("\tstart:");Serial.print(this->Memory.currentStartTime);

        Serial.println();
        Serial.print("\tDAY :");
        Serial.print("\tDst:");Serial.print(this->Memory.dayDistance/1000);
        Serial.print("\tAvg:");Serial.print(this->Memory.dayAverageSpeed);
        Serial.print("\tMax:");Serial.print(this->Memory.dayMaxSpeed);
        Serial.print("\tTime:");Serial.print(this->Memory.dayTime);
        Serial.print("\tTotal:");Serial.print(this->getDayTotalTime());
        Serial.print("\tstart:");Serial.print(this->Memory.dayStartTime);

        Serial.println();
        Serial.print("\tPREV:");
        Serial.print("\tDst:");Serial.print(this->Memory.prevDistance/1000);
        Serial.print("\tAvg:");Serial.print(this->Memory.prevAverageSpeed);
        Serial.print("\tMax:");Serial.print(this->Memory.prevMaxSpeed);

        // Serial.print("totalDistance: ");Serial.print(this->Memory.totalDistance);
        // Serial.print("tripDistance: ");Serial.print(this->Memory.tripDistance/ 1000);
        // Serial.print("wheelCircumference: ");Serial.print(this->Memory.wheelCircumference);

        Serial.println("\r\r");
    }

    void speedTrigger()
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

    void cadansTrigger()
    {
        unsigned long now = millis();
        unsigned long cadansTime = now - this->lastCadansTimeMs;
        if (cadansTime > MIN_SENSOR_TIME)
        {
            this->lastCadansTimeMs = now;
            // Shift sensor times
            for (size_t i = 0; i < SENSOR_BUFF - 1; i++)
            {
                this->cadansTimesMs[i] = this->cadansTimesMs[i + 1];
            }
            // Add new sensor time
            this->cadansTimesMs[SENSOR_BUFF - 1] = cadansTime;
        }
    }


};
