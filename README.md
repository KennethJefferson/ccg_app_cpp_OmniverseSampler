# Omniverse - Creative Sample Transformer

A JUCE-based VST3/AU sampler plugin that transforms simple one-shots into complex, evolving soundscapes. Inspired by the "Multiverse" plugin by HZE Audio.

![Theme: Violet/Pink Neon](https://img.shields.io/badge/Theme-Violet%2FPink%20Neon-8B5CF6)
![C++20](https://img.shields.io/badge/C%2B%2B-20-blue)
![JUCE 7.0.12](https://img.shields.io/badge/JUCE-7.0.12-orange)
![License: MIT](https://img.shields.io/badge/License-MIT-green)

## Features

### Sample Engine
- **5 Independent Sample Slots** - Drag and drop WAV, AIFF, MP3, FLAC, or OGG files
- **Interactive Waveform Display** - Visual feedback with draggable in/out points
- **Per-Slot Controls**:
  - Volume (-60dB to +12dB)
  - Pitch (±24 semitones)
  - ADSR Envelope (Attack, Decay, Sustain, Release)
  - In/Out Points (trim samples visually)

### Playback Modes
- **Layer** - Play all loaded samples simultaneously
- **Random** - Randomly select one sample per note
- **Random Octave** - Add random octave transposition
- **Reverse** - Play samples backwards

### Per-Slot Filters
- State Variable Filter (Low Pass, High Pass, Band Pass)
- Cutoff frequency with resonance control
- LFO modulation (Sine, Triangle, Square, Sample & Hold)

### Global Effects Chain
1. **BBD Delay** - Authentic Bucket Brigade Device modeling
   - Clock-rate dependent filtering (longer delays = darker tone)
   - Companding (mu-law compression/expansion)
   - Soft saturation in feedback path
   - Modulation LFO for chorus-like effects

2. **BBD Chorus** - Stereo chorus with BBD character
   - Dual delay lines with 90° phase offset
   - Rate and depth controls
   - Tone shaping

3. **Tape Saturation** - Analog tape warmth
   - Asymmetric waveshaping (even harmonics)
   - Soft knee compression
   - High-frequency rolloff
   - Subtle bass enhancement

4. **Spectral Filter** - 3-band EQ with stereo spread
   - Low/Mid/High frequency bands
   - Per-band gain (±12dB)
   - Stereo spread control (mono lows, wide highs)

### Master Section
- Master Volume
- Stereo Width (mid-side processing)

## Building

### Requirements
- CMake 3.22 or higher
- C++20 compatible compiler
- JUCE 7.0.12 (automatically fetched)

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/yourusername/ccg_app_cpp_OmniverseSampler.git
cd ccg_app_cpp_OmniverseSampler

# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build VST3
cmake --build build --config Release --target Omniverse_VST3

# Build Standalone (for testing)
cmake --build build --config Release --target Omniverse_Standalone
```

### Output Locations
- **VST3**: `build/Omniverse_artefacts/Release/VST3/Omniverse.vst3`
- **Standalone**: `build/Omniverse_artefacts/Release/Standalone/Omniverse`

## Usage

See [Usage.md](Usage.md) for detailed usage instructions.

## Screenshots

The plugin features a dark theme with violet/purple and hot pink accent colors for a modern neon aesthetic.

## License

MIT License - see LICENSE file for details.

## Credits

- Built with [JUCE Framework](https://juce.com/)
- Inspired by [Multiverse by HZE Audio](https://hzeaudio.com/)
- Created with assistance from Claude AI

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.
