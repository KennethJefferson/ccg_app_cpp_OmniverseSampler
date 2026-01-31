# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2026-01-31

### Added
- Initial release of Omniverse VST Sampler

#### Core Sampler
- 5 independent sample slots with drag-and-drop support
- Supported formats: WAV, AIFF, AIF, MP3, FLAC, OGG
- Per-slot volume control (-60dB to +12dB)
- Per-slot pitch control (±24 semitones)
- Per-slot ADSR envelope (Attack, Decay, Sustain, Release)
- Interactive in/out points on waveform display with draggable handles
- Waveform visualization with violet/pink gradient

#### Playback Modes
- Layer mode (all samples play simultaneously)
- Random mode (one random sample per note)
- Random octave mode (random octave transposition)
- Reverse mode (backwards playback)

#### Per-Slot Filters
- State Variable Filter (Low Pass, High Pass, Band Pass)
- Cutoff frequency (20Hz - 20kHz)
- Resonance control (0-100%)
- Filter bypass option
- LFO modulation with 4 waveforms (Sine, Triangle, Square, Sample & Hold)
- LFO rate (0.01-20Hz) and depth controls

#### Global Effects
- **BBD Delay**: Authentic Bucket Brigade Device emulation
  - Clock-rate dependent filtering
  - Companding (mu-law compression/expansion)
  - Soft saturation in feedback path
  - Modulation LFO for chorus-like effects
  - Time, Feedback, Mod Depth, Mod Rate, Tone, Mix controls

- **BBD Chorus**: Stereo chorus with BBD characteristics
  - Dual delay lines with 90° phase offset
  - Rate, Depth, Tone, Mix controls

- **Tape Saturation**: Analog tape warmth emulation
  - Asymmetric waveshaping for even harmonics
  - Soft knee compression
  - Tape head high-frequency rolloff
  - Subtle bass enhancement
  - Drive, Compression, Tone, Mix controls

- **Spectral Filter**: 3-band spectral EQ
  - Low/Mid/High frequency bands (±12dB)
  - Per-band stereo spread
  - Soft limiting to prevent clipping
  - Low, Mid, High, Spread, Mix controls

#### Master Section
- Master volume control
- Stereo width (mid-side processing)

#### User Interface
- Dark theme with violet (#8B5CF6) and hot pink (#FF006E) accents
- Tabbed interface (Filters / Effects)
- Interactive waveform display with in/out point handles
- Resizable window (800x600 to 1920x1080)

#### Technical
- JUCE 7.0.12 framework
- CMake build system with FetchContent
- VST3, AU, and Standalone targets
- C++20 standard
- State saving/loading for all parameters and sample paths

### Fixed
- Audio not triggering correctly on sample drop (added voice active check)
- Choppy audio due to envelope timing tied to sample position (separated envelope time)

### Changed
- Theme colors from orange to violet/pink neon aesthetic
