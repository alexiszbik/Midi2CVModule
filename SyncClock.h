#pragma once 

#include "daisy.h"
using namespace daisy;

#include "Clock.h"
#include "TempoFinder.h"

namespace MIDIClockRes {
    uint8_t WholeNote = 96;
    uint8_t HalfNote = 48;
    uint8_t TripletHalfNote = 32;
    uint8_t Quarter = 24;
    uint8_t TripletQuarter = 16;
    uint8_t Eight = 12;
    uint8_t TripletEight = 8;
    uint8_t Sixteenth = 6;
    uint8_t TripletSixteenth = 4;
    uint8_t HalfSixteenth = 3;
    uint8_t TripletHalfSixteenth = 2;
}

class SyncClock : public Clock {
public:
    SyncClock(HIDLed* clockLed) : Clock(clockLed) {}

    void setClockState(bool state) {
        this->state = state;
        clockLed->setState(state);
    }

    void tickFromMidi() {
        int res = MIDIClockRes::Sixteenth;
        bool clockState = (tick % res) < (res/2);
        setClockState(clockState && isPlaying);

        if (tick % MIDIClockRes::Quarter == 0) {
            tempoFinder.tickQuarter();
        }

        tick++;

        if (tick >= MIDIClockRes::Quarter * 4) {
            tick = 0;
        }   
    }

    void Start() {
        tick = 0;
        isPlaying = true;
    }

    void Stop() {
        setClockState(false);
        isPlaying = false;
    }


    bool process(float rateKnobValue) {
        return state;
    }

    bool getState() {
        return state;
    }

private:
    bool state = false;
    uint8_t tick = 0;
    bool isPlaying = false;
    TempoFinder tempoFinder;

};