#pragma once 

#include "daisy.h"
using namespace daisy;

#include "Clock.h"

class FreeClock : public Clock {
public:
    FreeClock(HIDLed* clockLed) : Clock(clockLed) {}

    bool process(uint32_t length) {
        uint32_t now = System::GetNow();
        if (now > (length + time)) {
            state = !state;
            clockLed->setState(state);
            time = now;
        }
        return state;
    }

private:
    uint32_t time = System::GetNow();

};