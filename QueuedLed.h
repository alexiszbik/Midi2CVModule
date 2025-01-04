#pragma once 

#include "daisy.h"
#include "HIDLed.h"

using namespace daisy;

struct QueuedLed {

    QueuedLed(HIDLed* led) {
        this->led = led;
    }

    void setOn() {
        led->setState(true);
        onTime = System::GetNow();
    }

    void process() {
        uint32_t now = System::GetNow();
        if (now > (onTime + length)) {
            led->setState(false);
        }
    }

private :
    HIDLed* led;
    const uint32_t length = 5;
    uint32_t onTime = System::GetNow();
};