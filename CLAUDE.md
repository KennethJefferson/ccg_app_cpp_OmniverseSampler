# Claude Code Instructions for Omniverse VST Sampler

## Project Overview
This is a JUCE-based VST3/AU sampler plugin called "Omniverse" - a creative sampler that transforms one-shots into complex soundscapes. It's a clone of the "Multiverse" plugin by HZE Audio.

## Tech Stack
- **Language**: C++20
- **Framework**: JUCE 7.0.12 (fetched via CMake FetchContent)
- **Build System**: CMake 3.22+
- **Targets**: VST3, AU, Standalone

## Project Structure
```
Source/
├── PluginProcessor.h/cpp    # Main audio processor
├── PluginEditor.h/cpp       # Main UI editor
├── DSP/                     # Digital Signal Processing
│   ├── BBDDelay.h           # Bucket Brigade Device delay
│   ├── BBDChorus.h          # BBD-based chorus
│   ├── TapeSaturation.h     # Analog tape saturation
│   ├── SpectralFilter.h     # 3-band spectral EQ
│   ├── SVFilter.h           # State Variable Filter
│   └── LFO.h                # Low Frequency Oscillator
├── Sampler/                 # Sample playback engine
│   ├── SampleSlot.h/cpp     # Individual sample slot
│   ├── OmniverseVoice.h/cpp # Synthesiser voice
│   └── OmniverseSampler.h/cpp # Main sampler
├── UI/                      # User interface components
│   ├── SlotPanel.h/cpp      # Per-slot control panel
│   ├── WaveformDisplay.h/cpp # Interactive waveform view
│   ├── FiltersPanel.h/cpp   # Filters tab UI
│   ├── EffectsPanel.h/cpp   # Effects tab UI
│   └── OmniverseLookAndFeel.h/cpp # Custom styling
└── Utils/
    └── Parameters.h/cpp     # APVTS parameter definitions
```

## Build Commands
```bash
# Configure (first time)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build VST3
cmake --build build --config Release --target Omniverse_VST3

# Build Standalone
cmake --build build --config Release --target Omniverse_Standalone
```

## Key Features
- 5 independent sample slots with drag-and-drop
- Per-slot: Volume, Pitch (±24 semitones), ADSR envelope
- Per-slot: SVF filters (LP/HP/BP) with LFO modulation
- Interactive in/out points on waveform display
- Playback modes: Layer, Random, Random Octave, Reverse
- Global effects chain: BBD Delay → BBD Chorus → Tape Saturation → Spectral Filter

## Theme Colors
- Primary: Violet/Purple `#8B5CF6`
- Accent: Hot Pink `#FF006E`
- Background: Dark `#0A0A0A`

## Coding Conventions
- Use JUCE coding style
- Header-only DSP classes for simplicity
- APVTS for all parameter management
- Timer-based UI updates (10Hz)

## Important Notes
- All effects are global (applied to mixed output), not per-slot
- Pitch shifting uses playback rate ratio (simple resampling)
- BBD modeling includes companding, clock-rate filtering, soft saturation
- In/Out points are stored as percentages (0-100%)
