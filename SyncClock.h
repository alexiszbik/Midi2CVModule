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
    SyncClock(HIDLed* clockLed) : Clock(clockLed) {
        rateCount = rateList.size();
    }

    void setClockState(bool state) {
        this->state = state;
        clockLed->setState(state);
    }

    void tickFromMidi() {
        int res = currentRate;
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
        int rateIndex = round(rateKnobValue*(rateCount-1));
        currentRate = rateList[rateIndex];
        return state;
    }

private:
    bool state = false;
    uint8_t tick = 0;
    bool isPlaying = false;
    TempoFinder tempoFinder;

    int rateCount;

    uint8_t currentRate = MIDIClockRes::Sixteenth;

    std::vector<uint8_t> rateList = {
        MIDIClockRes::WholeNote, 
        MIDIClockRes::HalfNote, 
        MIDIClockRes::TripletHalfNote, 
        MIDIClockRes::Quarter, 
        MIDIClockRes::TripletQuarter, 
        MIDIClockRes::Eight, 
        MIDIClockRes::TripletEight, 
        MIDIClockRes::Sixteenth, 
        MIDIClockRes::TripletSixteenth, 
        MIDIClockRes::HalfSixteenth, 
        MIDIClockRes::TripletHalfSixteenth
    };

};