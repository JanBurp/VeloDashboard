#pragma once

#include "Arduino.h"
#include "TeensyTimerTool.h"

#define INDICATORS_OFF      0
#define INDICATORS_LEFT     1
#define INDICATORS_RIGHT    2
#define INDICATORS_BOTH     4

#define INDICATOR_TIMER          500ms
#define ALARM_TIMER              250ms

class IndicatorClass {

    private:
        int function;
        bool stateLeft;
        bool stateRight;
        TeensyTimerTool::PeriodicTimer timer;

	public:

		void init() {
            this->timer.begin( [this] { this->loop(); } , INDICATOR_TIMER, false );
            this->reset();
		}

        void reset() {
            this->function = INDICATORS_OFF;
            this->stateLeft = false;
            this->stateRight = false;
            this->timer.stop();
        }

		void set(int function) {
            if ( function != this->function ) {
                this->reset();
                this->function = function;
                if ( this->function == INDICATORS_BOTH ) {
                    this->timer.setPeriod( ALARM_TIMER );
                }
                else {
                    this->timer.setPeriod( INDICATOR_TIMER );
                }
                this->loop();
                this->timer.start();
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

        void setAlarm() {
            this->setBoth();
        }

        void toggleAlarm() {
            if ( this->isAlarmSet() ) {
                this->reset();
            }
            else {
                this->setAlarm();
            }
        }

        bool isAlarmSet() {
            return this->function == INDICATORS_BOTH;
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
            }
            if ( this->function == INDICATORS_RIGHT || this->function == INDICATORS_BOTH ) {
                this->stateRight = ! this->stateRight;
            }
        }


};
