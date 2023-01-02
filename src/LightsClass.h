#pragma once

#include "Arduino.h"

#include "settings.h"

#include "BatteryClass.h"
#include "LedClass.h"

#define LIGHTS_OFF 0
#define LIGHTS_DIM 1
#define LIGHTS_NORMAL 2
#define LIGHTS_BEAM 3
#define REARLIGHTS_OFF 0
#define REARLIGHTS_DIM 1
#define REARLIGHTS_NORMAL 2
#define REARLIGHTS_FOG 3
#define REARLIGHTS_BREAK 3


class LightsClass
{

private:
    BatteryClass *Battery;
    LedClass *HeadLightLeft, *HeadLightRight, *RearLight, *BrakeLight;
    int lights = LIGHTS_OFF;
    int rearLights = REARLIGHTS_OFF;
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
        int lights = this->lights;
        int rear = this->rearLights;
        if ( this->Battery->isVeryLow() or this->Battery->isAlmostDead() ) {
            if ( lights>=LIGHTS_BEAM ) {
                lights--;
            }
            if ( rear>=REARLIGHTS_FOG ) {
                rear--;
            }
        }

        if ( this->horn ) {
            this->HeadLightLeft->setIntensity(HEAD_LED_MAX_INTENSITY);
            this->HeadLightRight->setIntensity(HEAD_LED_MAX_INTENSITY);
        }
        else {
            switch (lights)
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


        switch (rear)
        {
            case REARLIGHTS_OFF:
                this->RearLight->setIntensity(REAR_LED_OFF_INTENSITY);
                break;
            case REARLIGHTS_DIM:
                this->RearLight->setIntensity(REAR_LED_LOW_INTENSITY);
                break;
            case REARLIGHTS_NORMAL:
                this->RearLight->setIntensity(REAR_LED_MEDIUM_INTENSITY);
                break;
            case REARLIGHTS_FOG:
                this->RearLight->setIntensity(REAR_LED_MAX_INTENSITY);
                this->BrakeLight->setIntensity(REAR_LED_MAX_INTENSITY);
                break;
        }

    }

    void resetLights() {
        this->lights = LIGHTS_OFF;
        this->rearLights = REARLIGHTS_DIM;
        this->_set();
    }

    void increaseLights()
    {
        this->lights++;
        if ( this->lights>LIGHTS_BEAM ) {
            this->lights = LIGHTS_OFF;
        }
        this->_set();
    }

    void increaseRearLights()
    {
        this->rearLights++;
        if ( this->rearLights > REARLIGHTS_FOG ) {
            this->rearLights = REARLIGHTS_DIM;
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
        this->rearLights = REARLIGHTS_OFF;
        this->_set();
    }

    int getLights()
    {
        if (this->horn && !this->Battery->isVeryLow() && !this->Battery->isAlmostDead() ) {
            return LIGHTS_BEAM;
        }
        return this->lights;
    }

    int getRearLights() {
        return this->rearLights;
    }

    bool getHorn()
    {
        return this->horn;
    }

};
