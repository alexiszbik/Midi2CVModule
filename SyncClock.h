#pragma once 

#include <stdint.h>
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
        clockLed->setState(state);
        this->state = state;
    }

    void tickFromMidi() {
        int res = currentRate;
        bool clockState = (tick % res) < (res/2);
        bool isOddStep = (tick % (res * 2)) >= res;

        if (clockState != internalState) {
            internalState = clockState;
            if (internalState) {
                uint32_t offset = 0;
                if (isOddStep && currentRate == MIDIClockRes::Sixteenth) { //Groove is only working for 16 steps rythms
                    float stepLen = (60.f/getTempo()) * ((float)res / 24.f) * (groove * 0.5f) ; // min groove = 0, max = 0.5
                    offset = stepLen * 1000;
                }
                
                nextClockTime = System::GetNow() + offset;

                scheduled = true;
            } else {
                setClockState(false);
            }
        }

        if (tick % MIDIClockRes::Sixteenth == 0) {
            tempoFinder.tickSixteenth();
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
        internalState = false; 
        setClockState(false);
        isPlaying = false;
    }

    bool process(float knobValue, float grooveValue, bool useTriplets) {

        this->groove = grooveValue;

        uint32_t time = System::GetNow();
        if (time >= nextClockTime && scheduled) {
            setClockState(isPlaying);
            scheduled = false;
        }

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
    bool internalState = false;
    bool state = false;
    bool scheduled = false;
    uint8_t tick = 0;
    bool isPlaying = false;
    TempoFinder tempoFinder;

    bool isOddStep = false;
    float groove = 0.0f;

    int rateCount;
    int rateCountNoTriplets;

    uint32_t nextClockTime = System::GetNow();

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