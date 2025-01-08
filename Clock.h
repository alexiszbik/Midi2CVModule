#pragma once

#include "daisy.h"
using namespace daisy;

#include "HIDLed.h"

class Clock {
public:
     Clock(HIDLed* clockLed) : clockLed(clockLed) {}

protected:
    HIDLed* clockLed;
    bool state = false;

};