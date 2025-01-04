#include "daisy_patch_sm.h"
#include "daisysp.h"

#include "TempoFinder.h"
#include "HIDLed.h"
#include "QueuedLed.h"
#include "FreeClock.h"

#define NOTE_CV_OUT CV_OUT_2
#define RISER_CV_OUT CV_OUT_1

#define RATE_CV_IN CV_6

#define FREE_RATE_KNOB CV_2
#define SYNC_RATE_KNOB CV_3
#define CHANCE_KNOB CV_4

#define MIN_MIDI_NOTE 36

using namespace daisy;
using namespace patch_sm;

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

MidiUartHandler midi;
DaisyPatchSM hardware;
TempoFinder tempoFinder;

Pin toggleClockType = hardware.D10;
Pin toggleNoTriplets = hardware.D9;
Pin toggleRise = hardware.D8;

HIDLed ledRise = HIDLed(hardware.D6);
HIDLed ledFreeClock = HIDLed(hardware.D1);
HIDLed ledRateClock = HIDLed(hardware.D5);
HIDLed ledMIDI = HIDLed(hardware.D4);
HIDLed ledNotes = HIDLed(hardware.D7);

QueuedLed* queuedLed;
FreeClock* freeClock;

uint8_t tick = 0;
bool isPlaying = false;

void InitMidi()
{
    MidiUartHandler::Config midi_config;
    midi_config.transport_config.rx = hardware.A9;
    midi.Init(midi_config);
}

void setSyncClock(bool state) {
    dsy_gpio_write(&hardware.gate_out_2, state);
    ledRateClock.setState(state);
}

void setGate(bool state) {
    dsy_gpio_write(&hardware.gate_out_1, state);
    ledNotes.setState(state);
}

void tickFromMidi() {
    int res = MIDIClockRes::Sixteenth;
    bool clockState = (tick % res) < (res/2);
    setSyncClock(clockState && isPlaying);

    if (tick % MIDIClockRes::Quarter == 0) {
        tempoFinder.tickQuarter();
    }

    tick++;

    if (tick >= MIDIClockRes::Quarter * 4) {
        tick = 0;
    }   
}

void HandleMidiMessage(MidiEvent m)
{
    if (m.type == SystemCommon) {
        return;
    }

    queuedLed->setOn();

    if (m.type == SystemRealTime) {
        switch (m.srt_type) {
            case TimingClock :
                tickFromMidi();
            break;
            case Start : {
                tick = 0;
                isPlaying = true;
            }
            break;
            case Stop :  {
                setSyncClock(false);
                setGate(false);
                isPlaying = false;
            }
            break;
            default : break;
        }
        
    }
    
    switch(m.type)
    {
        case NoteOn:
        {
            NoteOnEvent p = m.AsNoteOn();
            hardware.SetLed(p.velocity > 0);
            setGate(p.velocity > 0);
            int minNote = MIN_MIDI_NOTE;
            int range = 5*12;
            hardware.WriteCvOut(NOTE_CV_OUT, ((float)(fmin(fmax(p.note, minNote), minNote + range) - minNote))/12.f);

        }
        break;
        case NoteOff:
        {
            NoteOffEvent p = m.AsNoteOff();
            hardware.SetLed(false);
            setGate(false);
        }
        break;

        default: break;
    }
}

int main(void)
{
    hardware.Init();
    queuedLed = new QueuedLed(&ledMIDI);
    freeClock = new FreeClock(&ledFreeClock);

    InitMidi();
    
    System::Delay(100);

    auto allLeds = {&ledRise, &ledFreeClock, &ledRateClock, &ledMIDI, &ledNotes};

    for (auto led : allLeds) {
        led->setState(true);
    }

    System::Delay(1000);

    for (auto led : allLeds) {
        led->setState(false);
    }
    midi.StartReceive();

    for(;;)
    {
        
        midi.Listen();
        while(midi.HasEvents())
        {
            HandleMidiMessage(midi.PopEvent());
        }

        queuedLed->process();
        freeClock->process();

        System::Delay(1);
    }
}
