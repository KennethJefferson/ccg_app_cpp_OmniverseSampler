# Omniverse Usage Guide

## Getting Started

### Installation

1. Build the plugin following the instructions in README.md
2. Copy the VST3 file to your DAW's plugin folder:
   - **Windows**: `C:\Program Files\Common Files\VST3\`
   - **macOS**: `/Library/Audio/Plug-Ins/VST3/`
3. Rescan plugins in your DAW
4. Insert Omniverse on a MIDI track

### Loading Samples

1. **Drag and Drop**: Simply drag audio files onto any of the 5 sample slots
2. **Supported Formats**: WAV, AIFF, AIF, MP3, FLAC, OGG
3. The waveform will display immediately after loading

## Interface Overview

### Header Section
- **Title**: "omniverse" with version number
- **Tabs**: Switch between "filters" and "effects" panels
- **Master Controls**: Master Volume and Stereo Width

### Sample Slots (Top Row)

Each of the 5 slots contains:

#### Waveform Display
- Shows the loaded sample waveform
- **IN/OUT Handles**: Drag to set playback range
  - Pink vertical lines indicate in/out points
  - Dimmed areas show excluded regions
  - Handles turn white when hovered

#### In/Out Controls
- **in**: Start point (0-100%)
- **out**: End point (0-100%)
- Can also be adjusted via sliders below the waveform

#### Volume & Pitch
- **volume**: -60dB to +12dB
- **pitch**: ±24 semitones (2 octaves up/down)

#### Envelope (ADSR)
- **attack**: 0-10000ms (time to reach peak)
- **decay**: 0-10000ms (time to reach sustain level)
- **sustain**: -60dB to 0dB (held level)
- **release**: 0-10000ms (fade out after note off)

### Playback Modes (Left Sidebar)

- **layer**: All loaded samples play together
- **random**: One random sample plays per note
- **random octave**: Adds random octave shift (-1, 0, or +1)
- **reverse**: Plays samples backwards

### Filters Tab

Per-slot filter controls:

- **Filter Type**: Low Pass, High Pass, or Band Pass
- **Cutoff**: 20Hz - 20kHz
- **Resonance**: 0-100%
- **Bypass**: Disable filter

#### LFO (modulates filter cutoff)
- **Rate**: 0.01 - 20Hz
- **Depth**: 0-100%
- **Waveform**: Sine, Triangle, Square, Sample & Hold

### Effects Tab

#### BBD Delay (Time Section)
- **time**: 1-1000ms delay time
- **feedback**: 0-99% (higher = more repeats)
- **mod depth**: LFO modulation amount
- **mod rate**: LFO speed (0.01-10Hz)
- **tone**: Dark to bright (0-100%)
- **mix**: Dry/wet balance
- **bypass**: Disable effect

#### BBD Chorus (Movement Section)
- **rate**: LFO speed (0.01-5Hz)
- **depth**: Modulation intensity
- **tone**: Brightness control
- **mix**: Dry/wet balance
- **bypass**: Disable effect

#### Tape Saturation (Color Section)
- **drive**: Input gain/saturation amount
- **compression**: Soft knee compression
- **tone**: High frequency rolloff
- **mix**: Dry/wet balance
- **bypass**: Disable effect

#### Spectral Filter (Master Section)
- **low**: Bass gain (±12dB)
- **mid**: Presence gain (±12dB)
- **high**: Air/brightness gain (±12dB)
- **spread**: Stereo width per band
- **mix**: Dry/wet balance
- **bypass**: Disable effect

## Tips & Tricks

### Creating Evolving Pads
1. Load sustained samples in multiple slots
2. Enable "layer" mode
3. Set different pitch values for each slot
4. Use long attack and release times
5. Apply BBD Chorus for movement

### Rhythmic Textures
1. Load percussive one-shots
2. Enable "random" mode
3. Set tight in/out points
4. Use short envelopes
5. Apply BBD Delay with rhythmic feedback

### Lo-Fi Character
1. Increase Tape Saturation drive
2. Roll off highs with Spectral Filter
3. Add subtle BBD Chorus
4. Use longer delay times (darker tone)

### Wide Stereo Image
1. Use Spectral Filter spread
2. Enable BBD Chorus
3. Increase master Stereo Width
4. Layer samples with slight pitch differences

## Keyboard Shortcuts

The plugin responds to standard DAW keyboard shortcuts. MIDI notes trigger all active slots (or random slot if enabled).

## Troubleshooting

### No Sound
- Check that samples are loaded (waveform visible)
- Verify MIDI is reaching the track
- Check master volume is not at -60dB
- Ensure at least one slot has a loaded sample

### Choppy Audio
- Increase buffer size in DAW settings
- Reduce polyphony if CPU usage is high

### Distortion
- Reduce individual slot volumes
- Lower master volume
- Reduce Tape Saturation drive
- Lower BBD Delay feedback
