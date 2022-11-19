#pragma once

#include "Arduino.h"

#include "BatteryClass.h"

#define LIGHTS_OFF 0
#define LIGHTS_DIM 1
#define LIGHTS_NORMAL 2
#define LIGHTS_BEAM 3
#define BACKLIGHTS_DIM 1
#define BACKLIGHTS_NORMAL 2
#define BACKLIGHTS_FOG 3
#define BACKLIGHTS_BREAK 3


class LightsClass
{

private:
    BatteryClass *Battery;
    int lights = LIGHTS_OFF;
    int backLights = BACKLIGHTS_DIM;
    bool brake = false;
    bool horn = false;

public:

    void init(BatteryClass *battery) {
        this->Battery = battery;
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
    }

    void decreaseLights()
    {
        if (this->lights > LIGHTS_OFF)
        {
            this->lights--;
        }
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
    }

    void decreaseBackLights()
    {
        if (this->backLights > BACKLIGHTS_DIM)
        {
            this->backLights--;
        }
    }

    void setBrake(bool b)
    {
        if ( !this->Battery->isVeryLow() && !this->Battery->isAlmostDead() ) {
            this->brake = b;
        }
    }

    bool getBrake()
    {
        return this->brake;
    }

    void setHorn(bool h) {
        this->horn = h;
    }

    bool getHorn()
    {
        return this->horn;
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

    void off() {
        this->lights = LIGHTS_OFF;
        this->backLights = BACKLIGHTS_DIM;
    }


};
