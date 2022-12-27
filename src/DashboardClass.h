#pragma once

#include "Arduino.h"

#define FN_INDICATOR_LEFT 0
#define FN_LIGHTS_MORE 1
#define FN_LIGHTS_LESS 2
#define FN_HORN 3
#define FN_DISPLAY 4
#define FN_INDICATOR_RIGHT 5
// combinations:
#define FN_ALARM 6             // indicator LEFT & RIGHT
#define FN_SHIFT FN_DISPLAY    // displat button is shift button
#define FN_BACKLIGHTS_MORE 7        // shift + more
#define FN_BACKLIGHTS_LESS 8        // shift + more
// important:
#define FN_BRAKE 10


#define DEBOUNCE    50
#define LONG_PRESS  1000


struct DashboardButton {
    int min;
    int max;
    int function;
};


DashboardButton Buttons[10] = {
    { 200, 300, FN_ALARM },                     // BOTH INDICATORS = 260
    { 305, 420, FN_INDICATOR_LEFT },            // 320
    { 440, 470, FN_BACKLIGHTS_LESS },           // 450
    { 475, 530, FN_INDICATOR_RIGHT },           // 490
    { 540, 590, FN_BACKLIGHTS_MORE },           // 570
    { 600, 680, FN_LIGHTS_LESS },               // 624
    { 685, 720, FN_DISPLAY },                   // 697
    { 725, 765, FN_LIGHTS_MORE },               // 742
    { 770, 815, FN_HORN },                      // RED = 780
    { 820, 900, FN_BRAKE },                     // 830
};

class DashboardClass {

    private:
        byte pin;
        bool debouncing;
        bool longPress;
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
		void init(byte pin, unsigned int debounceTime = DEBOUNCE) {

			this->pin = pin;
            this->debouncing = false;
            this->longPress = false;
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
                // else {
                //     if ( millis() - this->lastPressed >= LONG_PRESS ) {
                //         this->longPress = true;
                //         stateChanged = true;
                //     }
                // }
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
            if (func==-1) {
                this->lastPressed = 0;
                this->longPress = false;
            }
            this->debouncing = false;
            this->indicator = 0;
            this->alarm = false;
            this->shift = false;
            this->hasUsedShift = false;
            if ( this->function == FN_BACKLIGHTS_MORE || this->function == FN_BACKLIGHTS_LESS ) {
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

        // bool isLongPress() {
        //     return this->longPress;
        // }

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
