#pragma once

#include "Arduino.h"

#include "settings.h"

#include "BatteryClass.h"
#include "LedClass.h"

#define LIGHTS_OFF 0
#define LIGHTS_DIM 1
#define LIGHTS_NORMAL 2
#define LIGHTS_BEAM 3
#define BACKLIGHTS_OFF 0
#define BACKLIGHTS_DIM 1
#define BACKLIGHTS_NORMAL 2
#define BACKLIGHTS_FOG 3
#define BACKLIGHTS_BREAK 3


class LightsClass
{

private:
    BatteryClass *Battery;
    LedClass *HeadLightLeft, *HeadLightRight, *RearLight, *BrakeLight;
    int lights = LIGHTS_OFF;
    int backLights = BACKLIGHTS_OFF;
    bool brake = false;
    bool horn = false;

public:

    void init(BatteryClass *battery, LedClass *left, LedClass *right, LedClass *rear, LedClass *brake) {
        this->Battery = battery;
        this->HeadLightLeft = left;
        this->HeadLightRight = right;
        this->RearLight = rear;
        this->BrakeLight = brake;
    }

    void _set() {
        if ( this->horn ) {
            this->HeadLightLeft->setIntensity(HEAD_LED_MAX_INTENSITY);
            this->HeadLightRight->setIntensity(HEAD_LED_MAX_INTENSITY);
        }
        else {
            switch (this->lights)
            {
                case LIGHTS_OFF:
                    this->HeadLightLeft->setIntensity(HEAD_LED_OFF_INTENSITY);
                    this->HeadLightRight->setIntensity(HEAD_LED_OFF_INTENSITY);
                    break;
                case LIGHTS_DIM:
                    this->HeadLightLeft->setIntensity(HEAD_LED_LOW_INTENSITY);
                    this->HeadLightRight->setIntensity(HEAD_LED_LOW_INTENSITY);
                    break;
                case LIGHTS_NORMAL:
                    this->HeadLightLeft->setIntensity(HEAD_LED_MEDIUM_INTENSITY);
                    this->HeadLightRight->setIntensity(HEAD_LED_MEDIUM_INTENSITY);
                    break;
                case LIGHTS_BEAM:
                    this->HeadLightLeft->setIntensity(HEAD_LED_MAX_INTENSITY);
                    this->HeadLightRight->setIntensity(HEAD_LED_MAX_INTENSITY);
                    break;
            }
        }

        if ( this->brake ) {
            this->BrakeLight->setIntensity(REAR_LED_MAX_INTENSITY);
        }
        else {
            this->BrakeLight->setIntensity(REAR_LED_OFF_INTENSITY);
        }

        switch (this->backLights)
        {
            case BACKLIGHTS_OFF:
                this->RearLight->setIntensity(REAR_LED_OFF_INTENSITY);
                break;
            case BACKLIGHTS_DIM:
                this->RearLight->setIntensity(REAR_LED_LOW_INTENSITY);
                break;
            case BACKLIGHTS_NORMAL:
                this->RearLight->setIntensity(REAR_LED_MEDIUM_INTENSITY);
                break;
            case BACKLIGHTS_FOG:
                this->RearLight->setIntensity(REAR_LED_MAX_INTENSITY);
                this->BrakeLight->setIntensity(REAR_LED_MAX_INTENSITY);
                break;
        }

    }

    void resetLights() {
        this->lights = LIGHTS_OFF;
        this->backLights = BACKLIGHTS_DIM;
        this->_set();
    }

    void increaseLights()
    {
        if (this->lights < LIGHTS_BEAM)
        {
            this->lights++;
            if ( this->lights==LIGHTS_BEAM and (this->Battery->isVeryLow() or this->Battery->isAlmostDead() ) ) {
                this->lights--;
            }
        }
        this->_set();
    }

    void decreaseLights()
    {
        if (this->lights > LIGHTS_OFF)
        {
            this->lights--;
        }
        this->_set();
    }

    void increaseBackLights()
    {
        if (this->backLights < BACKLIGHTS_FOG)
        {
            this->backLights++;
            if ( this->backLights==BACKLIGHTS_FOG and (this->Battery->isVeryLow() or this->Battery->isAlmostDead() ) ) {
                this->backLights--;
            }
        }
        this->_set();
    }

    void decreaseBackLights()
    {
        if (this->backLights > BACKLIGHTS_DIM)
        {
            this->backLights--;
        }
        this->_set();
    }

    void setBrake(bool b)
    {
        if ( !this->Battery->isVeryLow() && !this->Battery->isAlmostDead() ) {
            this->brake = b;
        }
        this->_set();
    }

    bool getBrake()
    {
        return this->brake;
    }

    void setHorn(bool h) {
        this->horn = h;
        this->_set();
    }

    void off() {
        this->lights = LIGHTS_OFF;
        this->backLights = BACKLIGHTS_OFF;
        this->_set();
    }

    int getLights()
    {
        if (this->horn && !this->Battery->isVeryLow() && !this->Battery->isAlmostDead() ) {
            return LIGHTS_BEAM;
        }
        return this->lights;
    }

    int getBackLights() {
        return this->backLights;
    }

    bool getHorn()
    {
        return this->horn;
    }

};
