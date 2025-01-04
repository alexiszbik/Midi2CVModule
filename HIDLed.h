#pragma once 

#include "daisy.h"
using namespace daisy;

struct HIDLed {
    HIDLed(Pin pin) {
        led.pin  = pin;
        led.mode = DSY_GPIO_MODE_OUTPUT_PP;
        led.pull = DSY_GPIO_NOPULL;
        dsy_gpio_init(&led);
    }

    void setState(bool state) {
        dsy_gpio_write(&led, state);
    }
    dsy_gpio led;
};