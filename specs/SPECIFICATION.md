# Omniverse - Creative Sampler VST Clone Specification

**Version:** 1.0
**Based on:** Multiverse by HZE Audio x Waves Crate
**Framework:** JUCE (C++)
**Target Formats:** VST3, AU, Standalone

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                        MIDI INPUT                                │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                    PLAYBACK MODE ROUTER                          │
│         (Layer / Random / Random Octave / Reverse)               │
└─────────────────────────────────────────────────────────────────┘
                              │
        ┌─────────┬─────────┬─┴─────────┬─────────┐
        ▼         ▼         ▼           ▼         ▼
    ┌───────┐ ┌───────┐ ┌───────┐ ┌───────┐ ┌───────┐
    │ Slot 1│ │ Slot 2│ │ Slot 3│ │ Slot 4│ │ Slot 5│
    │       │ │       │ │       │ │       │ │       │
    │ Pitch │ │ Pitch │ │ Pitch │ │ Pitch │ │ Pitch │
    │ ADSR  │ │ ADSR  │ │ ADSR  │ │ ADSR  │ │ ADSR  │
    │ Filter│ │ Filter│ │ Filter│ │ Filter│ │ Filter│
    │ LFO   │ │ LFO   │ │ LFO   │ │ LFO   │ │ LFO   │
    └───┬───┘ └───┬───┘ └───┬───┘ └───┬───┘ └───┬───┘
        │         │         │         │         │
        └─────────┴─────────┴────┬────┴─────────┘
                                 │
                                 ▼
┌─────────────────────────────────────────────────────────────────┐
│                      GLOBAL EFFECTS CHAIN                        │
│   ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐        │
│   │   TIME   │──│ MOVEMENT │──│  COLOR   │──│  MASTER  │        │
│   │BBD Delay │  │BBD Chorus│  │Tape Sat  │  │Spectral  │        │
│   │ Stutter  │  │          │  │          │  │ Filter   │        │
│   └──────────┘  └──────────┘  └──────────┘  └──────────┘        │
│                    (Re-orderable chain)                          │
└─────────────────────────────────────────────────────────────────┘
                                 │
                                 ▼
┌─────────────────────────────────────────────────────────────────┐
│                    MASTER OUTPUT                                 │
│              (Volume + Stereo Width)                             │
└─────────────────────────────────────────────────────────────────┘
```

---

## MVP Phases

### Phase 1: Core Sampler (No Effects)

**Goal:** Load samples, trigger via MIDI, basic mixing

#### Features
- [ ] JUCE project scaffolding with CMake
- [ ] 5 independent sample slots
- [ ] Drag-and-drop WAV loading per slot
- [ ] Per-slot controls:
  - Volume: -inf to +12 dB
  - Pitch: ±24 semitones (real-time pitch shifting)
  - ADSR Envelope (Attack, Decay, Sustain, Release)
- [ ] Playback modes:
  - Layer: All loaded slots trigger simultaneously
  - Random: One random slot triggers per note
  - Random Octave: Occasional ±1 octave transposition
  - Reverse: Play samples backwards
- [ ] Basic waveform display per slot
- [ ] MIDI note triggering (middle C = root pitch)
- [ ] AudioProcessorValueTreeState (APVTS) for all parameters

#### Technical Details
- Use `juce::Synthesiser` with custom `SamplerVoice` class
- Voice allocation: 16 voices max (configurable)
- Sample rate conversion on load
- Memory: Load samples into `juce::AudioBuffer<float>`

#### Parameter Specifications

| Parameter | Range | Default | Unit |
|-----------|-------|---------|------|
| slot_X_volume | -60 to +12 | 0 | dB |
| slot_X_pitch | -24 to +24 | 0 | semitones |
| slot_X_attack | 0 to 10000 | 10 | ms |
| slot_X_decay | 0 to 10000 | 100 | ms |
| slot_X_sustain | -60 to 0 | -6 | dB |
| slot_X_release | 0 to 10000 | 500 | ms |
| playback_mode | 0-2 | 0 (Layer) | enum |
| random_octave | 0/1 | 0 | bool |
| reverse | 0/1 | 0 | bool |

---

### Phase 2: Filters + LFO

**Goal:** Per-slot filtering with modulation

#### Features
- [ ] Per-slot filter section:
  - Filter Type: LP / HP / BP toggle
  - Cutoff: 20 Hz - 20 kHz
  - Resonance: 0 - 1 (Q factor)
- [ ] Per-slot LFO:
  - Rate: 0.01 Hz - 20 Hz
  - Depth: 0 - 100%
  - Waveform: Sine / Triangle / Square / S&H
  - Destination: Filter Cutoff
- [ ] Filter bypass per slot

#### Technical Details
- Use `juce::dsp::StateVariableTPTFilter` (or custom SVF)
- LFO runs at control rate (every 32-64 samples)
- Smooth parameter changes to avoid zipper noise

#### Parameter Specifications

| Parameter | Range | Default | Unit |
|-----------|-------|---------|------|
| slot_X_filter_type | 0-2 | 0 (LP) | enum |
| slot_X_filter_cutoff | 20 to 20000 | 10000 | Hz |
| slot_X_filter_resonance | 0 to 1 | 0.1 | normalized |
| slot_X_lfo_rate | 0.01 to 20 | 1 | Hz |
| slot_X_lfo_depth | 0 to 1 | 0 | normalized |
| slot_X_lfo_waveform | 0-3 | 0 (Sine) | enum |
| slot_X_filter_bypass | 0/1 | 0 | bool |

---

### Phase 3: BBD Delay (Authentic Modeling)

**Goal:** First global effect - authentic Bucket Brigade emulation

#### Features
- [ ] BBD Delay module in TIME section
- [ ] Parameters:
  - Time: 1 ms - 1000 ms
  - Feedback: 0 - 100%
  - Mod Depth: 0 - 1 (chorus-like modulation)
  - Mod Rate: 0.01 - 10 Hz
  - Tone: Low-pass filter on delay line
  - Mix: 0 - 100% (wet/dry)
- [ ] BBD-specific characteristics:
  - Clock noise / artifacts at low clock rates
  - Per-stage low-pass filtering (bandwidth reduction)
  - Soft saturation on feedback path
  - Companding (compression/expansion) artifacts

#### Technical Details

**BBD Theory:**
A Bucket Brigade Device passes signal through a chain of capacitors (stages). Each stage introduces:
1. Sample-and-hold behavior
2. Low-pass filtering (Nyquist limit = clock_rate / 2)
3. Noise accumulation
4. Slight nonlinearity

**Implementation Approach:**
```cpp
// Pseudo-structure
class BBDDelayLine {
    // Model 1024-stage or 4096-stage BBD (like MN3005/MN3205)
    int numStages = 1024;
    float clockRate; // Derived from delay time

    // Per-stage LPF (simplified as single LPF at Nyquist)
    float lpfCutoff = clockRate / 2.0f;

    // Companding (pre-emphasis + de-emphasis)
    Compander compander;

    // Soft saturation
    float saturate(float x) {
        return std::tanh(x * drive);
    }
};
```

**Clock Rate Calculation:**
```
delay_time_seconds = numStages / (2 * clockRate)
clockRate = numStages / (2 * delay_time_seconds)
```

For 1024 stages at 200ms delay:
```
clockRate = 1024 / (2 * 0.2) = 2560 Hz
Nyquist = 1280 Hz (explains the "dark" BBD sound)
```

#### Parameter Specifications

| Parameter | Range | Default | Unit |
|-----------|-------|---------|------|
| delay_time | 1 to 1000 | 213 | ms |
| delay_feedback | 0 to 1 | 0.79 | normalized |
| delay_mod_depth | 0 to 1 | 0.88 | normalized |
| delay_mod_rate | 0.01 to 10 | 0.91 | Hz |
| delay_tone | 0 to 1 | 0.33 | normalized |
| delay_mix | 0 to 1 | 0.42 | normalized |
| delay_bypass | 0/1 | 0 | bool |

---

### Phase 4: BBD Chorus (Movement)

**Goal:** Stereo chorus effect using BBD modeling

#### Features
- [ ] BBD Chorus module in MOVEMENT section
- [ ] Parameters:
  - Depth: Modulation amount
  - Rate: LFO speed
  - Stereo Width: Phase offset between L/R
  - Mix: Wet/dry blend

#### Technical Details
- Dual BBD delay lines (L/R) with short delay times (1-20ms)
- LFO modulates delay time
- Stereo: 90° or 180° LFO phase offset between channels

---

### Phase 5: Tape Saturation (Color)

**Goal:** Analog warmth and harmonic distortion

#### Features
- [ ] Tape Saturation module in COLOR section
- [ ] Parameters:
  - Character: Bias/type of saturation curve
  - Drive: Input gain into saturation
  - Tone: Post-saturation EQ
  - Output: Makeup gain
  - Mix: Parallel blend

#### Technical Details
- Waveshaping with asymmetric soft-clipping
- Optional oversampling (2x-4x) to reduce aliasing
- Hysteresis modeling for tape-like response (optional, CPU-heavy)

---

### Phase 6: Spectral Filter (Master)

**Goal:** Dual-band frequency isolation

#### Features
- [ ] Spectral Filter in MASTER section
- [ ] Parameters:
  - Left Cutoff: High-pass frequency
  - Right Cutoff: Low-pass frequency
  - Link toggle: Lock both cutoffs together
- [ ] Visual frequency display

#### Technical Details
- High-order filters (12-24 dB/oct) for steep rolloff
- Consider linear-phase option for mastering use

---

### Phase 7: Stutter Effect (Time)

**Goal:** Buffer freeze/repeat effect

#### Features
- [ ] Stutter module in TIME section
- [ ] Parameters:
  - Division: Beat-synced repeat length (1/4, 1/8, 1/16, etc.)
  - Gate: On/off or triggered
  - Pitch decay: Each repeat drops in pitch

#### Technical Details
- Circular buffer captures incoming audio
- On trigger, loops the buffer at specified division
- Optional pitch-shifting per repeat

---

### Phase 8: Effect Chain Reordering

**Goal:** User-configurable effect signal flow

#### Features
- [ ] Re-order toggles per effect module
- [ ] Presets for common chains:
  - Serial (default): Time → Movement → Color → Master
  - Parallel: Effects summed together
  - Custom drag-and-drop ordering

---

### Phase 9: UI Polish

**Goal:** Match Multiverse aesthetic

#### Features
- [ ] Dark theme (near-black background)
- [ ] Orange/amber waveform displays
- [ ] Minimal borders, clean typography
- [ ] Numerical value displays (not traditional knobs)
- [ ] Envelope curve visualizers
- [ ] Spectral filter visualization

---

### Phase 10: Presets & Randomization

**Goal:** Preset management and creative randomization

#### Features
- [ ] Save/Load preset system
- [ ] Randomize button with dropdown:
  - Randomize All
  - Randomize Envelopes
  - Randomize Filters
  - Randomize Effects
- [ ] Weighted randomization (musically useful ranges)

---

## State Management

All parameters managed via `AudioProcessorValueTreeState`:

```cpp
// Parameter ID naming convention
// Slots: slot_1_volume, slot_2_pitch, etc.
// Effects: delay_time, chorus_depth, etc.
// Global: master_volume, stereo_width, playback_mode

juce::AudioProcessorValueTreeState apvts;
```

**Preset Format:** XML or JSON containing full APVTS state + sample file paths (relative)

---

## Performance Targets

| Metric | Target |
|--------|--------|
| CPU (idle, no samples) | < 1% |
| CPU (5 slots playing, all effects) | < 15% |
| Latency | 0 samples (no lookahead) |
| Memory per slot (1min stereo 44.1k) | ~21 MB |
| Plugin scan time | < 500ms |

---

## File Structure (Proposed)

```
Omniverse/
├── CMakeLists.txt
├── Source/
│   ├── PluginProcessor.h/cpp      # Main audio processor
│   ├── PluginEditor.h/cpp         # Main UI
│   ├── Sampler/
│   │   ├── OmniverseSampler.h/cpp # juce::Synthesiser wrapper
│   │   ├── OmniverseVoice.h/cpp   # Custom SamplerVoice
│   │   └── SampleSlot.h/cpp       # Per-slot state & DSP
│   ├── DSP/
│   │   ├── BBDDelay.h/cpp         # Bucket Brigade Delay
│   │   ├── BBDChorus.h/cpp        # BBD-based chorus
│   │   ├── TapeSaturation.h/cpp   # Saturation processor
│   │   ├── SpectralFilter.h/cpp   # Dual HP/LP
│   │   ├── Stutter.h/cpp          # Buffer freeze effect
│   │   └── EffectChain.h/cpp      # Re-orderable chain manager
│   ├── UI/
│   │   ├── WaveformDisplay.h/cpp  # Sample visualization
│   │   ├── EnvelopeView.h/cpp     # ADSR curve display
│   │   ├── SlotPanel.h/cpp        # Per-slot UI
│   │   ├── EffectsPanel.h/cpp     # Effects tab UI
│   │   ├── FiltersPanel.h/cpp     # Filters tab UI
│   │   └── LookAndFeel.h/cpp      # Custom styling
│   └── Utils/
│       ├── Parameters.h           # Parameter definitions
│       └── Presets.h/cpp          # Preset management
├── Resources/
│   └── (fonts, images if any)
└── Tests/
    ├── SamplerTests.cpp
    └── DSPTests.cpp
```

---

## Open Questions / Decisions Needed

1. **Polyphony:** Should each slot be independently polyphonic, or is it one voice per note across all slots?
   - **Recommended:** One voice triggers all 5 slots (or selected slot in Random mode)

2. **Sample streaming vs. preload:** For long samples, stream from disk or load entirely into RAM?
   - **Recommended:** Full preload for MVP (simpler), streaming as optimization later

3. **Pitch shifting algorithm:**
   - Basic resampling (changes length)
   - Time-stretch preserving length (granular/phase vocoder)
   - **Recommended:** Basic resampling for MVP (pitch = playback speed ratio)

4. **Host tempo sync:** Should LFO rates and delay times sync to DAW tempo?
   - **Recommended:** Add as option in later phase

---

## References

- [JUCE Documentation](https://docs.juce.com/)
- [BBD Delay Theory](https://www.electrosmash.com/mn3007-bucket-brigade-device)
- [MN3005 Datasheet](typical 4096-stage BBD specs)
- Multiverse original: https://www.multiverse-authentication.com/work/multiverse/
