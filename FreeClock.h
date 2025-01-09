#pragma once 

#include "daisy.h"
using namespace daisy;

#include "Clock.h"

class FreeClock : public Clock {
public:
    FreeClock(HIDLed* clockLed) : Clock(clockLed) {}

    bool process(float knobValue) {
        knobValue = fmax(0, knobValue);

        uint32_t length = (uint32_t)(knobValue*knobValue*knobValue * (maxLength - minLength));
        length = length + (uint32_t)minLength;
        uint32_t now = System::GetNow();
        if (now > (length + time)) {
            state = !state;
            clockLed->setState(state);
            time = now;
        }
        return state;
    }

private:
    const float minLength = 5;
    const float maxLength = 2000;

    uint32_t time = System::GetNow();

};