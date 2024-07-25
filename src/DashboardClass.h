#pragma once

#include "Arduino.h"

#define SWITCH_BRAKE    0
#define BUTTON_LEFT     1
#define BUTTON_DOWN     2
#define BUTTON_UP       3
#define BUTTON_ALARM    4
#define BUTTON_DISPLAY  5
#define BUTTON_RIGHT    6

#define NUM_BUTTONS 7

#define BRAKE_THRESHOLD 250 // 930
#define LOW_THRESHOLD 100 // 7
#define MID_THRESHOLD 175 // 147 // 695
#define HIGH_THRESHOLD 275 // 198 // 800

// Hack to make high resistors work (for first press)
#define START_THRESHOLD 500
#define START_MID_THRESHOLD 725
#define START_HIGHT_THRESHOLD 900

#define DEBOUNCE 50
#define LONG_PRESS 1500

struct AnalogValue
{
    byte pin;
    int value;
};

AnalogValue Values[3];

struct DashboardButton
{
    byte pin;
    int min;
    int max;
    int startmin;
    int startmax;
    int button;
};

DashboardButton Buttons[NUM_BUTTONS];

class DashboardClass
{

private:
    byte pinBrake;
    byte pinButtonsL;
    byte pinButtonsR;
    bool debouncing;
    bool longPress;
    unsigned int debounceTime;
    unsigned long lastPressed;
    int pressedButton;

public:
    /**
     * Setup the button, specifying and optional debounce delay
     */
    void init(byte brake, byte left, byte right, unsigned int debounceTime = DEBOUNCE)
    {
        this->pinBrake = brake;
        this->pinButtonsL = left;
        this->pinButtonsR = right;

        Values[0] = {this->pinBrake, 0};
        Values[1] = {this->pinButtonsL, 0};
        Values[2] = {this->pinButtonsR, 0};

        Buttons[0] = {0, 0, BRAKE_THRESHOLD, 0, BRAKE_THRESHOLD, SWITCH_BRAKE};

        Buttons[1] = {2, MID_THRESHOLD, HIGH_THRESHOLD, START_MID_THRESHOLD, START_HIGHT_THRESHOLD, BUTTON_RIGHT};
        Buttons[2] = {2, LOW_THRESHOLD, MID_THRESHOLD, LOW_THRESHOLD, START_MID_THRESHOLD, BUTTON_DISPLAY};
        Buttons[3] = {2, 0, LOW_THRESHOLD, 0, LOW_THRESHOLD, BUTTON_ALARM};

        Buttons[4] = {1, 0, LOW_THRESHOLD, 0, LOW_THRESHOLD, BUTTON_UP};
        Buttons[5] = {1, LOW_THRESHOLD, MID_THRESHOLD, LOW_THRESHOLD, START_MID_THRESHOLD, BUTTON_DOWN};
        Buttons[6] = {1, MID_THRESHOLD, HIGH_THRESHOLD, START_MID_THRESHOLD, START_HIGHT_THRESHOLD, BUTTON_LEFT};


        this->debouncing = false;
        this->longPress = false;
        this->debounceTime = debounceTime;
        this->lastPressed = 0;
        this->pressedButton = -1;
    }

    void _readValues()
    {
        for (size_t i = 0; i < 3; i++)
        {
            Values[i].value = analogRead(Values[i].pin);
        }
        if (DEBUG)
        {
            Serial.print("Brake:\t");
            Serial.print(Values[0].value);
            Serial.print("     \tLeft:\t");
            Serial.print(Values[1].value);
            Serial.print("     \tRight:\t");
            Serial.print(Values[2].value);
            Serial.print("     \r");
        }
    }

    bool _setButtonAccordingToValues()
    {
        bool stateChanged = false;

        int button = -1;
        for (size_t i = 0; i < NUM_BUTTONS; i++)
        {
            int value = Values[Buttons[i].pin].value;
            if (value > START_THRESHOLD) {
                if (value >= Buttons[i].startmin && value < Buttons[i].startmax)
                {
                    button = Buttons[i].button;
                }
            }
            else {
                if (value >= Buttons[i].min && value < Buttons[i].max)
                {
                    button = Buttons[i].button;
                }
            }
        }

        if (button >= 0)
        {
            if (button != this->pressedButton)
            {
                if (!this->debouncing)
                {
                    this->debouncing = true;
                    this->lastPressed = millis(); // Remember time for debouncing
                }
                else
                {
                    if (millis() - this->lastPressed >= this->debounceTime)
                    {
                        stateChanged = this->_setPressedButton(button);
                    }
                }
            }
            else
            {
                if (!this->longPress && (millis() - this->lastPressed >= LONG_PRESS))
                {
                    this->longPress = true;
                    stateChanged = true;
                }
            }
        }
        else
        {
            this->_setPressedButton(-1);
        }
        return stateChanged;
    }

    bool _setPressedButton(int button)
    {
        if (button == -1)
        {
            this->lastPressed = 0;
            this->longPress = false;
        }
        this->debouncing = false;
        this->pressedButton = button;

        if (DEBUG && this->pressedButton >= 0)
        {
            Serial.println();
            Serial.print("\tBUTTON:\t");
            Serial.print(this->pressedButton);
            Serial.println();
        }

        return true;
    }

    /**
     * Read buttons state, TRUE if state is changed
     */
    bool read()
    {
        this->_readValues();
        return this->_setButtonAccordingToValues();
    }

    bool waitForButtonPress()
    {
        while (!this->read())
        {
            ;
        }
        return true;
    }

    int getPressedButton()
    {
        return this->pressedButton;
    }

    bool isPressedButton(int button)
    {
        return this->pressedButton == button;
    }

    bool isLongPressed()
    {
        return this->longPress;
    }

    bool isLongPressedButton(int button)
    {
        if (!this->isLongPressed())
        {
            return false;
        }
        return this->pressedButton == button;
    }

    bool isIndicatorLeft()
    {
        return this->isPressedButton(BUTTON_LEFT);
    }

    bool isIndicatorRight()
    {
        return this->isPressedButton(BUTTON_RIGHT);
    }

    bool isLightsUp()
    {
        return (this->pressedButton == BUTTON_UP);
    }

    bool isLightsDown()
    {
        return (this->pressedButton == BUTTON_DOWN);
    }

    bool isBeam()
    {
        return (this->pressedButton == BUTTON_ALARM);
    }

    bool isDisplay()
    {
        return (this->pressedButton == BUTTON_DISPLAY);
    }

    bool isBrake()
    {
        return (this->pressedButton == SWITCH_BRAKE);
    }
};
