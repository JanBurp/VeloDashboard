#pragma once

#include "Arduino.h"

#include "settings.h"

#include "BatteryClass.h"
#include "LedClass.h"

#define LIGHTS_OFF 0
#define LIGHTS_DIM 1
#define LIGHTS_NORMAL 2
#define LIGHTS_BEAM 3
#define LIGHTS_FOG 4

class LightsClass
{

private:
    BatteryClass *Battery;
    LedClass *HeadLightLeft, *HeadLightRight, *RearLight;
    int lights = LIGHTS_OFF;
    bool brake = false;
    bool horn = false;

public:

    void init(BatteryClass *battery, LedClass *left, LedClass *right, LedClass *rear, LedClass *brake) {
        this->Battery = battery;
        this->HeadLightLeft = left;
        this->HeadLightRight = right;
        this->RearLight = rear;
        // this->BrakeLight = brake;
    }

    void _set() {
        int lights = this->lights;
        if ( this->Battery->isVeryLow() or this->Battery->isAlmostDead() ) {
            if ( lights>=LIGHTS_BEAM ) {
                lights = LIGHTS_NORMAL;
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
                    this->RearLight->setIntensity(REAR_LED_OFF_INTENSITY);
                    break;
                case LIGHTS_DIM:
                    this->HeadLightLeft->setIntensity(HEAD_LED_LOW_INTENSITY);
                    this->HeadLightRight->setIntensity(HEAD_LED_LOW_INTENSITY);
                    this->RearLight->setIntensity(REAR_LED_LOW_INTENSITY);
                    break;
                case LIGHTS_NORMAL:
                    this->HeadLightLeft->setIntensity(HEAD_LED_MEDIUM_INTENSITY);
                    this->HeadLightRight->setIntensity(HEAD_LED_MEDIUM_INTENSITY);
                    this->RearLight->setIntensity(REAR_LED_MEDIUM_INTENSITY);
                    break;
                case LIGHTS_BEAM:
                    this->HeadLightLeft->setIntensity(HEAD_LED_MAX_INTENSITY);
                    this->HeadLightRight->setIntensity(HEAD_LED_MAX_INTENSITY);
                    this->RearLight->setIntensity(REAR_LED_MEDIUM_INTENSITY);
                    break;
                case LIGHTS_FOG:
                    this->HeadLightLeft->setIntensity(HEAD_LED_MAX_INTENSITY);
                    this->HeadLightRight->setIntensity(HEAD_LED_MAX_INTENSITY);
                    this->RearLight->setIntensity(REAR_LED_MAX_INTENSITY);
                    break;
            }
        }

        if ( this->brake ) {
            this->RearLight->setIntensity(REAR_LED_MAX_INTENSITY);
        }

    }

    void resetLights() {
        this->lights = LIGHTS_OFF;
        this->_set();
    }

    void increaseLights()
    {
        this->lights++;
        if ( this->lights > LIGHTS_BEAM ) {
            this->lights = LIGHTS_BEAM;
        }
        this->_set();
    }

    void decreaseLights()
    {
        this->lights--;
        if ( this->lights < LIGHTS_OFF ) {
            this->lights = LIGHTS_OFF;
        }
        this->_set();
    }

    void setFogLight() {
        this->lights = LIGHTS_FOG;
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
        this->_set();
    }

    int getLights()
    {
        if (this->horn && !this->Battery->isVeryLow() && !this->Battery->isAlmostDead() ) {
            return LIGHTS_BEAM;
        }
        return this->lights;
    }

    bool getHorn()
    {
        return this->horn;
    }

};
