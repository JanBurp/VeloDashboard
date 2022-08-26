#ifndef Speed_h
#define Speed_h

#include "Arduino.h"

class Speed {

    private:
        byte pin;

        bool started = false;
        bool SpeedSensor = false;
        unsigned long lastSensorTimeMs = 0;
        unsigned long SensorTimeMs = 0;

        unsigned long startTimeMs = 0;
        unsigned long tripTimeMs = 0;

        // all in meters or meters/per hour
        float distance = 0.0;
        float speed = 0.0;
        float maxSpeed = 0.0;
        float avgSpeed = 0.0;

        float omtrek = 1.4; // meters

    public:

		void init(byte pin) {
			this->pin = pin;
            pinMode(this->pin, INPUT_PULLUP);
		}

        float getSpeed() {
            return round(this->speed * 10) / 10.0;
        }

        float getAvgSpeed() {
            return round(this->avgSpeed * 10) / 10.0;
        }

        float getMaxSpeed() {
            return round(this->maxSpeed * 10) / 10.0;
        }

        float getDistance() {
            return this->distance;
        }

		void loop() {

            bool sensor = digitalRead(this->pin);
            unsigned long now = millis();

            if (sensor != this->SpeedSensor)
            {
                this->SpeedSensor = sensor;

                if ( ! this->started )
                {
                    this->lastSensorTimeMs = now;
                    this->startTimeMs = now;
                }

                if ( this->SpeedSensor )
                {
                    this->distance += this->omtrek;
                    this->tripTimeMs = now - this->startTimeMs;
                    this->SensorTimeMs = now - this->lastSensorTimeMs;
                    this->lastSensorTimeMs = now;

                    float Speed_meter_sec = this->omtrek / (this->SensorTimeMs / 1000.0);
                    this->speed = Speed_meter_sec * 3.6;

                    if ( this->speed < 10000000 && this->speed >= this->maxSpeed )
                    {
                        this->maxSpeed = this->speed;
                    }
                    this->avgSpeed = this->distance / ( this->tripTimeMs / 1000.0) * 3.6;

                    Serial.print("Distance: \t");Serial.print(this->distance);
                    Serial.print("\tSpeed m: \t");Serial.print(Speed_meter_sec);
                    Serial.print("\tSpeed: \t");Serial.print(this->speed);
                    Serial.print("\tAvg Speed: \t");Serial.print(this->avgSpeed);
                    Serial.print("\tMax Speed: \t");Serial.print(this->maxSpeed);
                    Serial.print("\tTime: \t");Serial.print(this->tripTimeMs);
                    Serial.println();

                }

                this->started = true;
            };
        }
};



#endif
