#pragma once

#include "Arduino.h"
#include "settings.h"

class IdleClass
{

private:
    volatile unsigned long timeLastAction;

public:

    void action()
    {
        this->timeLastAction = millis();
    }

    unsigned long lastAction() {
        return this->timeLastAction;
    }

    int idleSeconds() {
        return (( millis() - this->timeLastAction ) / 1000);
    }

    int remainingSeconds() {
        return IDLE_TIMER - this->idleSeconds();
    }

    float remainingPercentage() {
        return float(this->remainingSeconds()) / float(IDLE_WARNING_TIMER);
    }

    bool warning() {
        return this->remainingSeconds() <= IDLE_WARNING_TIMER;
    }

    bool ended() {
        return this->idleSeconds() >= IDLE_TIMER;
    }


};
