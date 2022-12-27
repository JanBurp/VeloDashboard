#pragma once

#include "Arduino.h"

#define FN_INDICATOR_LEFT 0
#define FN_LIGHTS 1
#define FN_HORN 2
#define FN_DISPLAY 3
#define FN_INDICATOR_RIGHT 4
// combinations:
#define FN_ALARM 5             // indicator LEFT & RIGHT
// #define FN_SHIFT FN_DISPLAY    // displat button is shift button
#define FN_BACKLIGHTS 6        // shift + more
// important:
#define FN_BRAKE 10


#define DEBOUNCE    25
#define LONG_PRESS  1000


struct DashboardButton {
    int min;
    int max;
    int function;
};


DashboardButton Buttons[10] = {
    { 200, 290, FN_BACKLIGHTS },            // YELLOW + WHITE   = 280
    { 300, 420, FN_LIGHTS },                // YELLOW           = 315
    { 435, 460, FN_ALARM },                 // BLUE + GREEN     = 448
    { 470, 510, FN_HORN },                  // RED              = 489
    { 600, 650, FN_INDICATOR_LEFT },        // BLUE             = 625
    { 680, 710, FN_INDICATOR_RIGHT },       // GREEN            = 698
    { 730, 750, FN_BRAKE },                 // BRAKE            = 741
    { 760, 800, FN_DISPLAY },               // WHITE            = 780

};

class DashboardClass {

    private:
        byte pin;
        bool debouncing;
        bool longPress;
        unsigned int debounceTime;
        unsigned long lastPressed;

        int function;
        // bool shift;
        // bool hasUsedShift;

        int indicator = 0;
        bool alarm = false;

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
            // this->shift = false;
            // this->hasUsedShift = false;
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
                            stateChanged = this->setFunction(func);
                        }
                    }
                }
                else {
                    if ( millis() - this->lastPressed >= LONG_PRESS ) {
                        this->longPress = true;
                        stateChanged = true;
                    }
                }
			} else {
                // if ( this->shift && !this->hasUsedShift ) {
                    // stateChanged = this->setFunction(FN_SHIFT);
                // }
                // else {
                    this->setFunction(-1);
                // }
			}

            return stateChanged;
		}

        bool setFunction(int func) {
            if (func==-1) {
                this->lastPressed = 0;
                this->longPress = false;
            }
            this->debouncing = false;
            this->indicator = 0;
            this->alarm = false;
            // this->shift = false;
            // this->hasUsedShift = false;
            // if ( this->function == FN_BACKLIGHTS ) {
            //     this->hasUsedShift = true;
            // }
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

        bool isLongPress() {
            return this->longPress;
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

        bool isLights() {
            return ( this->function == FN_LIGHTS );
        }

        bool isBackLights() {
            return ( this->function == FN_BACKLIGHTS );
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
