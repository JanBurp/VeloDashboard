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
        float distance = 0;
        float speed = 0;
        float maxSpeed = 0;
        float avgSpeed = 0;

        float omtrek = 1.4; // meters

    public:

		void init(byte pin) {
			this->pin = pin;
            pinMode(this->pin, INPUT_PULLUP);
		}

		bool loop() {

            bool sensor = digitalRead(SPEED_INPUT);
            if (sensor != this->SpeedSensor)
            {
                this->SpeedSensor = sensor;

                if ( ! this->started )
                {
                    this->lastSensorTimeMs = millis();
                    this->startTimeMs = millis();
                }

                if (this->SpeedSensor)
                {
                    this->distance += this->omtrek;
                    this->tripTimeMs = millis() - this->startTimeMs;
                    this->SensorTime = millis() - this->lastSensorTimeMs;
                    this->lastSensorTimeMs = millis();

                    float Speed_meter_sec = (this->SensorTimeMs / 1000.0) * this->omtrek;
                    this->speed = Speed_meter_sec / 3600.0;

                    if (this->speed < 10000000 && this->speed >= this->maxSpeed)
                    {
                        this->maxSpeed = this->speed;
                    }
                    this->avgSpeed = this->distance / this->tripTimeMs;

                }

                this->started = true;
            };
        }



#endif
