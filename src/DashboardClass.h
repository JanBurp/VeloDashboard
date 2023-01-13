#pragma once

#include "Arduino.h"

#define FN_LEFT             0
#define FN_RIGHT            1
#define FN_UP               2
#define FN_DOWN             3
#define FN_HORN             4
#define FN_DISPLAY          5
#define FN_BRAKE            10

#define DEBOUNCE    50
#define LONG_PRESS  1500


struct DashboardButton {
    int min;
    int max;
    int function;
};


DashboardButton Buttons[10] = {
    { 0,    99, FN_BRAKE },
    { 100, 225, FN_HORN },                      // 178
    { 250, 350, FN_DISPLAY },                   // 301
    { 370, 470, FN_RIGHT },                     // 426
    { 475, 600, FN_LEFT },                      // 550
    { 625, 750, FN_DOWN },                      // 707
    { 775, 900, FN_UP },                        // 832
};

class DashboardClass {

    private:
        byte pin;
        bool debouncing;
        bool longPress;
        unsigned int debounceTime;
        unsigned long lastPressed;
        int function;
        int indicator = 0;

	public:

		/**
		 * Setup the button, specifying and optional debounce delay
		 */
		void init(byte pin, unsigned int debounceTime = DEBOUNCE) {
			this->pin = pin;
            this->debouncing = false;
            this->longPress = false;
			this->debounceTime = debounceTime;
			this->lastPressed = 0;
            this->function = -1;
		}

		/**
		 * Read buttons state, TRUE if state is changed
		 */
		bool read() {
            bool stateChanged = false;

			int value = analogRead(this->pin);

            // if ( DEBUG ) {
            //     Serial.print("\tValue:\t");
            //     Serial.print(value);
            //     Serial.println();
            // }

            int func = -1;
            for (int i = 0; i < 10; i++) {
                if ( value >= Buttons[i].min && value < Buttons[i].max ) {
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
                            stateChanged = this->setFunction(func);
                        }
                    }
                }
                else {
                    if ( !this->longPress && (millis() - this->lastPressed >= LONG_PRESS) ) {
                        this->longPress = true;
                        stateChanged = true;
                    }
                }
			} else {
                this->setFunction(-1);
			}

            return stateChanged;
		}

        bool setFunction(int func) {
            if (func==-1) {
                this->lastPressed = 0;
                this->longPress = false;
            }
            this->debouncing = false;
            this->function = func;
            this->indicator = 0;
            if ( this->function == FN_LEFT ) {
                this->indicator = -1;
            }
            if ( this->function == FN_RIGHT ) {
                this->indicator = 1;
            }

            if ( DEBUG && this->function!=-1 ) {
                Serial.print("\tFUNCTION:\t");
                Serial.print(this->function);
                Serial.println();
            }

            return true;
        }

        int getFunction() {
            return this->function;
        }

        bool isLongPress() {
            return this->longPress;
        }

        bool isIndicatorLeft() {
            return ( this->indicator==-1 );
        }

        bool isIndicatorRight() {
            return ( this->indicator==1 );
        }

        bool isLightsUp() {
            return ( this->function == FN_UP );
        }

        bool isLightsDown() {
            return ( this->function == FN_DOWN );
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
