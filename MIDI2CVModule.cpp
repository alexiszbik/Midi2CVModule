#include "daisy_patch_sm.h"
#include "daisysp.h"

// Use the daisy namespace to prevent having to type
// daisy:: before all libdaisy functions
using namespace daisy;
using namespace patch_sm;

MidiUartHandler midi;

// Declare a DaisySeed object called hardware
DaisyPatchSM hardware;

void InitMidi()
{
    MidiUartHandler::Config midi_config;
    midi_config.transport_config.rx = hardware.A9;
    midi.Init(midi_config);
}


void HandleMidiMessage(MidiEvent m)
{
    if (m.type == SystemRealTime || m.type == SystemCommon) {
        return;
    }
    
    switch(m.type)
    {
        case NoteOn:
        {
            NoteOnEvent p = m.AsNoteOn();
            hardware.SetLed(p.velocity > 0);
            dsy_gpio_write(&hardware.gate_out_1, p.velocity > 0);
            int minNote = 36;
            int range = 5*12;
            hardware.WriteCvOut(CV_OUT_2, ((float)(fmin(fmax(p.note, minNote), minNote + range) - minNote))/12.f);

        }
        break;
        case NoteOff:
        {
            NoteOffEvent p = m.AsNoteOff();
            hardware.SetLed(false);
            dsy_gpio_write(&hardware.gate_out_1, false);
        }
        break;

        default: break;
    }
}

int main(void)
{
    // Declare a variable to store the state we want to set for the LED.
    bool led_state;
    led_state = true;

    // Configure and Initialize the Daisy Seed
    // These are separate to allow reconfiguration of any of the internal
    // components before initialization.
    //hardware.Configure();
    hardware.Init();

    InitMidi();
    midi.StartReceive();

    // Loop forever
    for(;;)
    {
        /*
        // Set the onboard LED
        hardware.SetLed(led_state);

        // Toggle the LED state for the next time around.
        led_state = !led_state;

        // Wait 500ms
        System::Delay(500);
        */
        midi.Listen();
        //hardware.SetLed(true);
        // Handle MIDI Events
        while(midi.HasEvents())
        {
            HandleMidiMessage(midi.PopEvent());
        }
        //wait 1 ms
        System::Delay(1);
    }
}
