#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace Parameters
{
    // Master parameters
    inline const juce::String MASTER_VOLUME = "master_volume";
    inline const juce::String STEREO_WIDTH = "stereo_width";

    // Playback mode parameters
    inline const juce::String PLAYBACK_LAYER = "playback_layer";
    inline const juce::String PLAYBACK_RANDOM = "playback_random";
    inline const juce::String RANDOM_OCTAVE = "random_octave";
    inline const juce::String REVERSE = "reverse";

    // Slot parameter ID generators
    inline juce::String slotVolume(int slot) { return "slot_" + juce::String(slot) + "_volume"; }
    inline juce::String slotPitch(int slot) { return "slot_" + juce::String(slot) + "_pitch"; }
    inline juce::String slotAttack(int slot) { return "slot_" + juce::String(slot) + "_attack"; }
    inline juce::String slotDecay(int slot) { return "slot_" + juce::String(slot) + "_decay"; }
    inline juce::String slotSustain(int slot) { return "slot_" + juce::String(slot) + "_sustain"; }
    inline juce::String slotRelease(int slot) { return "slot_" + juce::String(slot) + "_release"; }
    inline juce::String slotInPoint(int slot) { return "slot_" + juce::String(slot) + "_in_point"; }
    inline juce::String slotOutPoint(int slot) { return "slot_" + juce::String(slot) + "_out_point"; }

    // Filter parameters (Phase 2)
    inline juce::String slotFilterType(int slot) { return "slot_" + juce::String(slot) + "_filter_type"; }
    inline juce::String slotFilterCutoff(int slot) { return "slot_" + juce::String(slot) + "_filter_cutoff"; }
    inline juce::String slotFilterResonance(int slot) { return "slot_" + juce::String(slot) + "_filter_resonance"; }
    inline juce::String slotFilterBypass(int slot) { return "slot_" + juce::String(slot) + "_filter_bypass"; }

    // LFO parameters (Phase 2)
    inline juce::String slotLfoRate(int slot) { return "slot_" + juce::String(slot) + "_lfo_rate"; }
    inline juce::String slotLfoDepth(int slot) { return "slot_" + juce::String(slot) + "_lfo_depth"; }
    inline juce::String slotLfoWaveform(int slot) { return "slot_" + juce::String(slot) + "_lfo_waveform"; }

    // BBD Delay parameters (Phase 3)
    inline const juce::String DELAY_TIME = "delay_time";
    inline const juce::String DELAY_FEEDBACK = "delay_feedback";
    inline const juce::String DELAY_MOD_DEPTH = "delay_mod_depth";
    inline const juce::String DELAY_MOD_RATE = "delay_mod_rate";
    inline const juce::String DELAY_TONE = "delay_tone";
    inline const juce::String DELAY_MIX = "delay_mix";
    inline const juce::String DELAY_BYPASS = "delay_bypass";

    // BBD Chorus parameters
    inline const juce::String CHORUS_RATE = "chorus_rate";
    inline const juce::String CHORUS_DEPTH = "chorus_depth";
    inline const juce::String CHORUS_TONE = "chorus_tone";
    inline const juce::String CHORUS_MIX = "chorus_mix";
    inline const juce::String CHORUS_BYPASS = "chorus_bypass";

    // Tape Saturation parameters
    inline const juce::String TAPE_DRIVE = "tape_drive";
    inline const juce::String TAPE_COMPRESSION = "tape_compression";
    inline const juce::String TAPE_TONE = "tape_tone";
    inline const juce::String TAPE_MIX = "tape_mix";
    inline const juce::String TAPE_BYPASS = "tape_bypass";

    // Spectral Filter parameters
    inline const juce::String SPECTRAL_LOW = "spectral_low";
    inline const juce::String SPECTRAL_MID = "spectral_mid";
    inline const juce::String SPECTRAL_HIGH = "spectral_high";
    inline const juce::String SPECTRAL_SPREAD = "spectral_spread";
    inline const juce::String SPECTRAL_MIX = "spectral_mix";
    inline const juce::String SPECTRAL_BYPASS = "spectral_bypass";

    // Number of slots
    constexpr int NUM_SLOTS = 5;

    // Create the full parameter layout
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
}
