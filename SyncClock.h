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
        rateCountNoTriplets = rateListNoTriplets.size();
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

    bool process(float knobValue, bool useTriplets) {

        knobValue = fmax(fmin(1.f - knobValue, 1.f), 0.f);

        if (useTriplets) {
            int rateIndex = round(knobValue*(rateCount-1));
            currentRate = rateList[rateIndex];
        } else {
            int rateIndex = round(knobValue*(rateCountNoTriplets-1));
            currentRate = rateListNoTriplets[rateIndex];
        }
        
        return state;
    }

    float getTempo() {
        return tempoFinder.getCurrentTempo();
    }

private:
    bool state = false;
    uint8_t tick = 0;
    bool isPlaying = false;
    TempoFinder tempoFinder;

    int rateCount;
    int rateCountNoTriplets;

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
    
    std::vector<uint8_t> rateListNoTriplets = {
        MIDIClockRes::WholeNote, 
        MIDIClockRes::HalfNote, 
        MIDIClockRes::Quarter, 
        MIDIClockRes::Eight, 
        MIDIClockRes::Sixteenth, 
        MIDIClockRes::HalfSixteenth, 
    };

};