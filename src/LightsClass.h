#pragma once

#include "Arduino.h"

#include "settings.h"

#include "BatteryClass.h"
#include "SpeedClass.h"
#include "LedClass.h"

#define LIGHTS_OFF 0
#define LIGHTS_DIM 1
#define LIGHTS_ON 2
#define LIGHTS_NORMAL 3
#define LIGHTS_BEAM 4
#define LIGHTS_FOG 5

class LightsClass
{

private:
    BatteryClass *Battery;
    SpeedClass *Speed;
    LedClass *HeadLightLeft, *HeadLightRight, *RearLight, *BrakeLight;
    int lights = LIGHTS_OFF;
    bool brake = false;
    bool beam = false;

public:

    void init(BatteryClass *battery, SpeedClass *speed, LedClass *left, LedClass *right, LedClass *rear, LedClass *brake) {
        this->Battery = battery;
        this->Speed = speed;
        this->HeadLightLeft = left;
        this->HeadLightRight = right;
        this->RearLight = rear;
        this->BrakeLight = brake;
    }

    void loop() {
        int lights = this->lights;
        if ( this->Battery->isVeryLow() or this->Battery->isAlmostDead() ) {
            if (lights == LIGHTS_FOG) {
                lights = LIGHTS_BEAM;
            }
        }

        this->BrakeLight->setIntensity(BRAKE_LEDS_OFF);

        if ( this->beam ) {
            this->HeadLightLeft->setIntensity(HEAD_LED_BEAM_INTENSITY);
            this->HeadLightRight->setIntensity(HEAD_LED_BEAM_INTENSITY);
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
                    this->HeadLightLeft->setIntensity(HEAD_LED_DIM_INTENSITY);
                    this->HeadLightRight->setIntensity(HEAD_LED_DIM_INTENSITY);
                    this->RearLight->setIntensity(REAR_LED_DIM_INTENSITY);
                    break;
                case LIGHTS_ON:
                    this->HeadLightLeft->setIntensity(HEAD_LED_ON_INTENSITY);
                    this->HeadLightRight->setIntensity(HEAD_LED_ON_INTENSITY);
                    this->RearLight->setIntensity(REAR_LED_ON_INTENSITY);
                    break;
                case LIGHTS_NORMAL:
                    this->HeadLightLeft->setIntensity(HEAD_LED_NORMAL_INTENSITY);
                    this->HeadLightRight->setIntensity(HEAD_LED_NORMAL_INTENSITY);
                    this->RearLight->setIntensity(REAR_LED_NORMAL_INTENSITY);
                    break;
                case LIGHTS_BEAM:
                    this->HeadLightLeft->setIntensity(HEAD_LED_BEAM_INTENSITY);
                    this->HeadLightRight->setIntensity(HEAD_LED_BEAM_INTENSITY);
                    this->RearLight->setIntensity(REAR_LED_BEAM_INTENSITY);
                    break;
                case LIGHTS_FOG:
                    this->HeadLightLeft->setIntensity(HEAD_LED_FOG_INTENSITY);
                    this->HeadLightRight->setIntensity(HEAD_LED_FOG_INTENSITY);
                    this->RearLight->setIntensity(REAR_LED_FOG_INTENSITY);
                    this->BrakeLight->setIntensity(BRAKE_LEDS_FOG);
                    break;
            }
        }

        if ( this->getBrake() ) {
            this->BrakeLight->setIntensity(BRAKE_LEDS_ON);
        }

    }

    void resetLights() {
        this->lights = LIGHTS_OFF;
    }

    void increaseLights()
    {
        this->lights++;
        if ( this->lights > LIGHTS_FOG ) {
            this->lights = LIGHTS_FOG;
        }
    }

    void decreaseLights()
    {
        this->lights--;
        if ( this->lights < LIGHTS_OFF ) {
            this->lights = LIGHTS_OFF;
        }
    }

    void setFogLight() {
        this->lights = LIGHTS_FOG;
    }

    void setBrake(bool b)
    {
        this->brake = b;
    }

    bool getBrake()
    {
        if (this->brake && !this->Battery->isAlmostDead()) {
            return true;
        }
        return false;
    }

    void setBeam(bool b) {
        this->beam = b;
    }

    void off() {
        this->lights = LIGHTS_OFF;
    }

    int getLights()
    {
        if (this->beam) {
            return LIGHTS_BEAM;
        }
        return this->lights;
    }

    bool getBeam()
    {
        return this->beam;
    }

};
