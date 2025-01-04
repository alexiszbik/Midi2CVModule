#pragma once 

#include "daisy.h"
using namespace daisy;

#include "HIDLed.h"

struct FreeClock {

    FreeClock(HIDLed* clockLed) : clockLed(clockLed) {}

    void process() {
        uint32_t length = 100;
        uint32_t now = System::GetNow();
        if (now > (length + time)) {
            state = !state;
            clockLed->setState(state);
            time = now;
        }
    }

private:
    HIDLed* clockLed;
    bool state = false;
    uint32_t time = System::GetNow();

};