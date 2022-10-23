#pragma once

#include "Arduino.h"

#define LIGHTS_OFF     0
#define LIGHTS_DIM     1
#define LIGHTS_NORMAL  2
#define LIGHTS_BEAM    3

class LightsClass {

    private:
      int lights = LIGHTS_OFF;
      bool brake = false;
      bool fog = false;

	public:

    void increaseLights() {
      if ( this->lights < LIGHTS_BEAM ) {
        this->lights++;
      }
    }

    void resetLights() {
      this->lights = LIGHTS_DIM;
    }

    void decreaseLights() {
      if ( this->lights > LIGHTS_OFF ) {
        this->lights--;
      }
    }

    void setBrake( bool b ) {
      this->brake = b;
    }

    void setFog( bool f ) {
      this->fog = f;
    }

    int getLights() {
      return this->lights;
    }

    bool getBrake() {
      return this->brake;
    }

    bool getFog() {
      return this->fog;
    }

};
