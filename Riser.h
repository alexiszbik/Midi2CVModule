#pragma once

#include "daisy.h"
using namespace daisy;

#include "QueuedLed.h"


class Riser {
    
public:
    Riser(HIDLed* led) : blink(led) {
    }

    float process(bool toggleState, float tempo) {
        float quarterLength = 60.f/tempo;
        float riserLength = 8.f * 4.f * quarterLength;

        if (currentState != toggleState) {
            currentState = toggleState;
            if (currentState) {
                time = System::GetNow();
            }
        }

        if (currentState) {
            blink->setState(true);
            auto now = System::GetNow();
            uint32_t delta = now - time;
            float fDelta = (float)delta * 0.001;
            return fDelta * 1.f/riserLength;
        } else {
            blink->setState(false);
            return 0;
        }
    }

private:
    HIDLed* blink;
    bool currentState = false;
    uint32_t time;
};