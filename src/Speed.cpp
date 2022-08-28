#ifndef Speed_h
#define Speed_h

#include "Arduino.h"

#define SENSOR_BUFF     5
#define MAX_SENSOR_TIME 1000
#define SPEED_CALC_TIME 100
#define MIN_SPEED       2

class Speed {

    private:
        byte pin;

        bool started = false;
        bool paused = true;
        unsigned long lastCalcedTime = 0;

        // Speed sensor
        bool SpeedSensor = true;
        unsigned long lastSensorTimeMs = 0;
        volatile unsigned long sensorTimesMs[SENSOR_BUFF] = {0};

        // Times in ms
        unsigned long startTimeMs = 0;
        unsigned long tripTimeMs = 0;

        // all in meters or meters/per hour
        float distance = 0.0;
        float speed = 0.0;
        float maxSpeed = 0.0;
        float avgSpeed = 0.0;

        float circumference = 1.450; // meters
        // 23-406 	1.420
        // 28-406 	1.450
        // 35-406 	1.510
        // 40-406 	1.540
        // 47-406 	1.580
        // 50-406 	1.600
        // 54-406 	1.620

    public:

		void init(byte pin) {
			this->pin = pin;
            pinMode(this->pin, INPUT_PULLUP);
		}

        bool isStarted() {
            return this->started;
        }

        float getSpeed(){
            return this->speed;
        }

        float getAvgSpeed() {
            return this->avgSpeed;
        }

        float getMaxSpeed() {
            return this->maxSpeed;
        }

        float getDistance() {
            return this->distance;
        }

        unsigned long getTripTime() {
            return this->tripTimeMs;
        }

        void loop() {
            bool sensor = digitalRead(this->pin);
            unsigned long now = millis();

            if ( this->started ) {
                this->tripTimeMs = now - this->startTimeMs;

                unsigned long calcedTime = now - this->lastCalcedTime;
                if (calcedTime >= SPEED_CALC_TIME) {
                    this->lastCalcedTime = now;

                    // Serial.print(this->lastSensorTimeMs);
                    // for (size_t i = 0; i < SENSOR_BUFF; i++)
                    // {
                    //     Serial.print("\t");
                    //     Serial.print(this->sensorTimesMs[i]);
                    // }
                    // Serial.println("");

                    // Calc sensor time average (only those that are larger than 0)
                    int buffLength = SENSOR_BUFF;
                    long totalSensorTime = 0;
                    for (size_t i = 0; i < SENSOR_BUFF; i++)
                    {
                        if ( this->sensorTimesMs[i]>0 ) {
                            totalSensorTime += this->sensorTimesMs[i];
                        }
                        else {
                            buffLength--;
                        }
                    }
                    long avgSensorTime = totalSensorTime / buffLength;

                    // Serial.println();
                    // Serial.print("total\t"); Serial.print(totalSensorTime);
                    // Serial.print("buff\t"); Serial.print(buffLength);
                    // Serial.print("avg\t"); Serial.println(avgSensorTime);

                    // Calc speeds
                    float Speed_meter_sec = this->circumference / (avgSensorTime / 1000.0);
                    this->speed = Speed_meter_sec * 3.6;
                    if ( this->speed < MIN_SPEED ) {
                        this->speed = 0.0;
                        this->paused = true;
                    }
                    else {
                        this->avgSpeed = this->distance / (this->tripTimeMs / 1000.0) * 3.6;
                        if (this->speed < 10000000 && this->speed >= this->maxSpeed) {
                            this->maxSpeed = this->speed;
                        }
                        this->paused = false;
                    }

                }
            }

            if (sensor != this->SpeedSensor) {
                this->SpeedSensor = sensor;

                if ( ! this->started ) {
                    this->startTimeMs = now;
                    this->lastSensorTimeMs = now;
                    this->started = true;
                }

                if ( this->SpeedSensor ) {
                    this->distance += this->circumference;
                    this->_add_to_sensor_buff(now - this->lastSensorTimeMs);
                    this->lastSensorTimeMs = now;
                }
            }

            // If stopped or slow, add long times...
            if ((now - this->lastSensorTimeMs) > MAX_SENSOR_TIME) {
                this->_add_to_sensor_buff(now - this->lastSensorTimeMs);
            }

            // Serial.print("Distance: \t");
            // Serial.print(this->distance);
            // Serial.print("\tSpeed: \t");
            // Serial.print(this->speed);
            // Serial.print("\tAvg Speed: \t");
            // Serial.print(this->avgSpeed);
            // Serial.print("\tMax Speed: \t");
            // Serial.print(this->maxSpeed);
            // Serial.print("\tTime: \t");
            // Serial.print(this->tripTimeMs);
            // Serial.println();
        }

        void _add_to_sensor_buff(unsigned long sensorTime ) {
            // Shift sensor times
            for (size_t i = 0; i < SENSOR_BUFF - 1; i++) {
                this->sensorTimesMs[i] = this->sensorTimesMs[i + 1];
            }
            this->sensorTimesMs[SENSOR_BUFF - 1] = sensorTime;
        }

};



#endif
