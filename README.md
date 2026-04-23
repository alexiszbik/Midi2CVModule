# MIDI to CV Module

Eurorack MIDI to CV converter built on the **Daisy Patch SM** platform. Converts MIDI note, CC, and clock messages into control voltages and gates for modular synthesizers.

## Features

- **1V/Oct pitch CV** output from MIDI notes (C2 to C7, 5 octaves)
- **Gate output** from Note On/Off
- **Clock output** with two modes: free-running or MIDI-synced
- **Extra CV output**: controlled via MIDI CC or random voltage on each clock tick
- **Groove/Swing** control on synced clock (sixteenth notes)
- **Probability** control on clock output
- **Mute** via MIDI CC

## MIDI Map

All note and CC messages are received on **MIDI Channel 2**.

| Message | Channel | Number | Effect |
|---|---|---|---|
| Note On | 2 | C2–C7 (36–96) | Pitch CV (1V/oct, 0–5V) + Gate high |
| Note Off | 2 | — | Gate low |
| CC | 2 | 13 | Extra CV output (0–5V, scaled from CC value 0–127). Only active when the Extra CV switch is in CC mode. |
| CC | 2 | 60 | Mute: value > 60 mutes the clock output, value <= 60 unmutes |
| MIDI Clock | — | — | Drives the synced clock (24 ppqn) |
| Start | — | — | Resets and starts the synced clock |
| Stop | — | — | Stops the synced clock and closes the gate |

## Outputs

| Output | Signal |
|---|---|
| Gate Out 1 | Note gate |
| Gate Out 2 | Clock gate |
| CV Out 1 | Extra CV (MIDI CC 13 or random, 0–5V) |
| CV Out 2 | Note pitch (1V/oct) |

## Controls

### Knobs

| Knob | Function |
|---|---|
| CV_3 | Free clock rate |
| CV_4 | Synced clock rate division |
| ADC_10 | Groove / swing amount (affects sixteenth notes only) |
| CV_2 | Clock probability (chance) |

### CV Input

| Input | Function |
|---|---|
| CV_6 | Rate modulation (subtracts from both free and synced clock rate knobs) |

### Toggles

| Toggle | Function |
|---|---|
| D10 | Clock source: free-running (off) / MIDI-synced (on) |
| D9 | Triplet rates: enabled (off) / disabled (on) |
| D8 | Extra CV mode: MIDI CC (off) / random on each clock tick (on) |

## Synced Clock Rate Divisions

Selectable via the sync rate knob. With triplets enabled:

Whole, Half, Triplet Half, Quarter, Triplet Quarter, Eighth, Triplet Eighth, Sixteenth, Triplet Sixteenth, Half-Sixteenth, Triplet Half-Sixteenth

With triplets disabled:

Whole, Half, Quarter, Eighth, Sixteenth, Half-Sixteenth

## LEDs

| Pin | Indicator |
|---|---|
| D1 | Free clock pulse |
| D5 | Synced clock pulse |
| D4 | MIDI activity |
| D6 | Extra CV activity |
| D7 | Note gate |

