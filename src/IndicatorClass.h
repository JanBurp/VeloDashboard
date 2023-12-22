#pragma once

#include "Arduino.h"
#include "settings.h"

#define INDICATORS_OFF      0
#define INDICATORS_LEFT     1
#define INDICATORS_RIGHT    2
#define INDICATORS_BOTH     4

PeriodicTimer indicatorTimer(TCK);

class IndicatorClass {

    private:
        int leftLED;
        int rightLED;
        int function;
        bool stateLeft;
        bool stateRight;

	public:

		void init(int left, int right) {
            this->leftLED = left;
            this->rightLED = right;
            pinMode(this->leftLED, OUTPUT);
            pinMode(this->rightLED, OUTPUT);
            indicatorTimer.begin( [this] { this->loop(); } , INDICATOR_TIMER, false );
            this->reset();
		}

        void reset() {
            this->function = INDICATORS_OFF;
            this->stateLeft = false;
            this->stateRight = false;
            digitalWrite(this->leftLED, this->stateLeft);
            digitalWrite(this->rightLED, this->stateRight);
            indicatorTimer.stop();
        }

		void set(int function) {
            if ( function == this->function || this->function == INDICATORS_BOTH ) {
                this->reset();
            }
            else {
                this->reset();
                this->function = function;
                if ( this->function == INDICATORS_BOTH ) {
                    indicatorTimer.setPeriod( ALARM_TIMER );
                }
                else {
                    indicatorTimer.setPeriod( INDICATOR_TIMER );
                }
                if ( this->function == INDICATORS_LEFT || this->function == INDICATORS_BOTH ) {
                    this->stateLeft = true;
                }
                if ( this->function == INDICATORS_RIGHT || this->function == INDICATORS_BOTH ) {
                    this->stateRight = true;
                }
                indicatorTimer.start();
                // this->loop();
            }
		}

        void setLeft() {
            this->set(INDICATORS_LEFT);
        }

        void setRight() {
            this->set(INDICATORS_RIGHT);
        }

        void setBoth() {
            this->set(INDICATORS_BOTH);
        }

        bool isActive() {
            return this->function > INDICATORS_OFF;
        }

        bool getStateLeft() {
            return this->stateLeft;
        }

        bool getStateRight() {
            return this->stateRight;
        }

        void loop() {
            if ( this->function == INDICATORS_LEFT || this->function == INDICATORS_BOTH ) {
                this->stateLeft = ! this->stateLeft;
                digitalWrite(this->leftLED, this->stateLeft);
            }
            if ( this->function == INDICATORS_RIGHT || this->function == INDICATORS_BOTH ) {
                this->stateRight = ! this->stateRight;
                digitalWrite(this->rightLED, this->stateRight);
            }
        }


};
