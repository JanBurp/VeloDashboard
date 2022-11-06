#pragma once

#include "Arduino.h"

#define FN_INDICATOR_LEFT 0
#define FN_LIGHTS_MORE 1
#define FN_LIGHTS_LESS 2
#define FN_HORN 3
#define FN_DISPLAY 4
#define FN_INDICATOR_RIGHT 5
// combinations:
#define FN_ALARM 6                  // indicator LEFT & RIGHT
#define FN_SHIFT FN_DISPLAY         // displat button is shift button
#define FN_BACKLIGHTS_MORE 7        // shift + more
#define FN_BACKLIGHTS_LESS 8        // shift + less
// important:
#define FN_BRAKE 10

struct DashboardButton {
    int min;
    int max;
    int function;
};

DashboardButton Buttons[10] = {
    { 200, 280, FN_ALARM },
    { 300, 420, FN_INDICATOR_LEFT },
    { 440, 470, FN_BACKLIGHTS_LESS },
    { 475, 530, FN_INDICATOR_RIGHT },
    { 540, 590, FN_BACKLIGHTS_MORE },
    { 600, 650, FN_LIGHTS_LESS },
    { 670, 720, FN_DISPLAY },
    { 730, 755, FN_LIGHTS_MORE },
    { 760, 800, FN_HORN },
    { 810, 900, FN_BRAKE },

};

class DashboardClass {

    private:
        byte pin;
        bool debouncing;
        unsigned int debounceTime;
        unsigned long lastPressed;

        int function;
        bool shift;
        bool hasUsedShift;

        int indicator = 0;
        bool alarm = false;

	public:

		/**
		 * Setup the button, specifying and optional debounce delay
		 */
		void init(byte pin, unsigned int debounceTime = 50) {

			this->pin = pin;
            this->debouncing = false;
			this->debounceTime = debounceTime;
			this->lastPressed = 0;
            this->function = -1;
            this->shift = false;
            this->hasUsedShift = false;
		}

		/**
		 * Read buttons state, TRUE if state is changed
		 */
		bool read() {
            bool stateChanged = false;

			int value = analogRead(this->pin);

            int func = -1;
            for (int i = 0; i < 10; i++) {
                if ( value > Buttons[i].min && value < Buttons[i].max ) {
                    func = Buttons[i].function;
                }
            }

			if ( func>=0 ) {
                if ( func!=this->function ) {
                    if ( !this->debouncing ) {
                        this->debouncing = true;
                        this->lastPressed = millis(); // Remember time for debouncing
                    }
                    else {
                        if (millis() - this->lastPressed >= this->debounceTime) {
                            if ( func==FN_SHIFT ) {
                                if ( this->function==FN_BACKLIGHTS_LESS || this->function==FN_BACKLIGHTS_MORE ) {
                                    this->setFunction(func);
                                }
                                this->shift = true;
                            }
                            else {
                                stateChanged = this->setFunction(func);
                            }
                        }
                    }
                }
			} else {
                if ( this->shift && !this->hasUsedShift ) {
                    stateChanged = this->setFunction(FN_SHIFT);
                }
                else {
                    this->setFunction(-1);
                }
			}

            return stateChanged;
		}

        bool setFunction(int func) {
            this->shift = false;
            this->hasUsedShift = false;
            this->lastPressed = 0;
            this->debouncing = false;
            this->alarm = false;
            this->indicator = 0;
            if ( this->function == FN_BACKLIGHTS_LESS || this->function == FN_BACKLIGHTS_MORE ) {
                this->hasUsedShift = true;
            }
            this->function = func;
            if ( this->function == FN_ALARM ) {
                this->alarm = true;
            }
            if ( this->function == FN_INDICATOR_LEFT ) {
                this->indicator = -1;
            }
            if ( this->function == FN_INDICATOR_RIGHT ) {
                this->indicator = 1;
            }
            return true;
        }

        bool isAlarm() {
            return this->alarm;
        }

        bool isIndicatorLeft() {
            return ( this->indicator==-1 );
        }

        bool isIndicatorRight() {
            return ( this->indicator==1 );
        }

        bool isLightsMore() {
            return ( this->function == FN_LIGHTS_MORE );
        }

        bool isLightsLess() {
            return ( this->function == FN_LIGHTS_LESS );
        }

        bool isBackLightsMore() {
            return ( this->function == FN_BACKLIGHTS_MORE );
        }

        bool isBackLightsLess() {
            return ( this->function == FN_BACKLIGHTS_LESS );
        }


        bool isHorn() {
            return ( this->function == FN_HORN );
        }

        bool isDisplay() {
            return ( this->function == FN_DISPLAY );
        }

        bool isBrake() {
            return ( this->function == FN_BRAKE );
        }


};
