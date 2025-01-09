#pragma once

#include "daisy.h"
using namespace daisy;

#include "QueuedLed.h"


class Riser {
    
public:
    Riser(HIDLed* led) : blink(led) {
    }

    void process(bool toggleState, float tempo) {
        if (toggleState) {
            blink->setState(true);
        } else {
            blink->setState(false);
        }
    }

private:
    HIDLed* blink;
};