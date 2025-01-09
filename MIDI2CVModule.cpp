#include "daisy_patch_sm.h"
#include "daisysp.h"

#include "HIDLed.h"
#include "QueuedLed.h"
#include "FreeClock.h"
#include "SyncClock.h"
#include "Riser.h"

#define NOTE_CV_OUT CV_OUT_2
#define RISER_CV_OUT CV_OUT_1

#define RATE_CV_IN CV_6

#define FREE_RATE_KNOB CV_3
#define SYNC_RATE_KNOB CV_4
#define CHANCE_KNOB CV_2

#define MIN_MIDI_NOTE 36

using namespace daisy;
using namespace patch_sm;

MidiUartHandler midi;
DaisyPatchSM hardware;

Pin toggleClockTypePin = hardware.D10;
Pin toggleNoTripletsPin = hardware.D9;
Pin toggleRisePin = hardware.D8;

Switch toggleClockType;
Switch toggleNoTriplets;
Switch toggleRise;

HIDLed ledRise = HIDLed(hardware.D6);
HIDLed ledFreeClock = HIDLed(hardware.D1);
HIDLed ledRateClock = HIDLed(hardware.D5);
HIDLed ledMIDI = HIDLed(hardware.D4);
HIDLed ledNotes = HIDLed(hardware.D7);

QueuedLed* queuedLed;
FreeClock* freeClock;
SyncClock* syncClock;
Riser* riser;

const float freeClockMinLength = 10;
const float freeClockMaxLength = 2000;

bool clockState = false;

void InitMidi()
{
    MidiUartHandler::Config midi_config;
    midi_config.transport_config.rx = hardware.A9;
    midi.Init(midi_config);
}

void setClock(bool state, float chance) {
    if (state != clockState) {
        float randValue = (float)rand() / (float)RAND_MAX;
        randValue *= 0.9f;
        if (chance >= randValue) {
            dsy_gpio_write(&hardware.gate_out_2, state);
        }
        clockState = state;
    }
}

void setGate(bool state) {
    dsy_gpio_write(&hardware.gate_out_1, state);
    ledNotes.setState(state);
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
                syncClock->tickFromMidi();
            break;
            case Start : {
                syncClock->Start();
            }
            break;
            case Stop :  {
                setGate(false);
                syncClock->Stop();
            }
            break;
            default : break;
        }
        
    }
    
    if (m.channel == 0) {
        switch(m.type)
        {
            case NoteOn:
            {
                NoteOnEvent p = m.AsNoteOn();
                hardware.SetLed(p.velocity > 0);
                setGate(p.velocity > 0);
                int minNote = MIN_MIDI_NOTE;
                int range = 5*12;
                float volt = ((float)(fmin(fmax(p.note, minNote), minNote + range) - minNote))/12.f;
                hardware.WriteCvOut(NOTE_CV_OUT,volt*1.024065f);
            }
            break;
            case NoteOff:
            {
                hardware.SetLed(false);
                setGate(false);
            }
            break;

            default: break;
        }
    }
}

int main(void)
{
    hardware.Init();
    queuedLed = new QueuedLed(&ledMIDI);
    freeClock = new FreeClock(&ledFreeClock);
    syncClock = new SyncClock(&ledRateClock);
    riser = new Riser(&ledRise);

    InitMidi();
    
    System::Delay(100);

    auto allLeds = {&ledRise, &ledFreeClock, &ledRateClock, &ledMIDI, &ledNotes};

    for (auto led : allLeds) {
        led->setState(true);
    }

    toggleClockType.Init(toggleClockTypePin);
    toggleNoTriplets.Init(toggleNoTripletsPin);
    toggleRise.Init(toggleRisePin);

    auto toggles = {&toggleClockType, &toggleNoTriplets, &toggleRise};

    System::Delay(1000);

    for (auto led : allLeds) {
        led->setState(false);
    }
    midi.StartReceive();

    for(;;)
    {
        for (auto toggle : toggles) {
             toggle->Debounce();
        }
        
        midi.Listen();
        while(midi.HasEvents())
        {
            HandleMidiMessage(midi.PopEvent());
        }

        hardware.ProcessAllControls();
        float freeKnobValue = hardware.GetAdcValue(FREE_RATE_KNOB);
        float syncKnobValue = hardware.GetAdcValue(SYNC_RATE_KNOB);
        float chanceKnobValue = hardware.GetAdcValue(CHANCE_KNOB);

        uint32_t freeLength = (uint32_t)(freeKnobValue*freeKnobValue*freeKnobValue * freeClockMaxLength);

        queuedLed->process();
        bool freeClockState = freeClock->process((uint32_t)freeClockMinLength + freeLength);

        bool syncState = syncClock->process(syncKnobValue, !toggleNoTriplets.Pressed());

        bool useFreeClock = !toggleClockType.Pressed();
        setClock(useFreeClock ? freeClockState : syncState, chanceKnobValue);

        float riserValue = riser->process(!toggleRise.Pressed(), syncClock->getTempo());

        hardware.WriteCvOut(RISER_CV_OUT,fmin(riserValue * 5.f, 5.0f));

        //System::Delay(1);
    }
}
