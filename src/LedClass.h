#pragma once

#include "Arduino.h"
#include <Arduino.h>

class LedClass
{
    private:
        volatile byte ledIntensity;
        byte pin;

    public:
    //     byte offIntensity = 0;    // "group" intensity
    //     byte lowIntensity = 0;    // "group" intensity
    //     byte mediumIntensity = 0; // "normal day ride"
    //     byte highIntensity = 0;   // "normal night ride"
    //     byte maxIntensity = 0;    // "max"


    void init(byte pin) {
        this->pin = pin;
        this->setIntensity(0);
    }

    void setIntensity(byte intensity)
    {
        this->ledIntensity = intensity;
        analogWrite(this->pin, this->ledIntensity);
        if ( DEBUG ) {
            Serial.print("PIN:\t");
            Serial.print(this->pin);
            Serial.print("\tIntensity:");
            Serial.print(this->ledIntensity);
            Serial.println();
        }

    }

    // // byte getLedIntensity() // return the current intensity of the led.
    // // {
    // //     return ledIntensity;
    // // }

    // void setOff()
    // {
    //     this->setIntensity(this->offIntensity);
    // }

    // void setLow()
    // {
    //     this->setIntensity(this->lowIntensit);
    // }

    // void setMedium()
    // {
    //     this->setIntensity(this->mediumIntensity);
    // }

    // void setHigh()
    // {
    //     this->setIntensity(this->highIntensity);
    // }

    // void setMax()
    // {
    //     this->setIntensity(this->maxIntensity);
    // }

    // void upLed()
    // {
    //     if (this->ledIntensity == this->offIntensity)
    //         this->setLow();
    //     else if (this->ledIntensity == this->lowIntensity)
    //         this->setMedium();
    //     else if (this->ledIntensity == this->mediumIntensity)
    //         this->setHigh();
    //     else if (this->ledIntensity == this->highIntensity)
    //         this->setMax();
    // }

    // void downLed()
    // {
    //     if (this->ledIntensity == this->lowIntensity)
    //         this->setOff();
    //     else if (this->ledIntensity == this->mediumIntensity)
    //         this->setLow();
    //     else if (this->ledIntensity == this->highIntensity)
    //         this->setMedium();
    //     else if (this->ledIntensity == this->maxIntensity)
    //         this->setHigh();
    // }
};
