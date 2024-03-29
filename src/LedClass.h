#pragma once

#include "Arduino.h"
#include <Arduino.h>

class LedClass
{
    private:
        volatile byte ledIntensity;
        byte pin;

    public:

    void init(byte pin) {
        this->pin = pin;
        this->setIntensity(0);
    }

    void setIntensity(byte intensity)
    {
        this->ledIntensity = intensity;
        analogWrite(this->pin, this->ledIntensity);
    }

};
