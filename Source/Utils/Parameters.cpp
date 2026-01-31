#include "Parameters.h"

namespace Parameters
{
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

        // Master parameters
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(MASTER_VOLUME, 1),
            "Master Volume",
            juce::NormalisableRange<float>(-60.0f, 12.0f, 0.1f),
            0.0f,
            juce::AudioParameterFloatAttributes().withLabel("dB")
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(STEREO_WIDTH, 1),
            "Stereo Width",
            juce::NormalisableRange<float>(0.0f, 200.0f, 1.0f),
            100.0f,
            juce::AudioParameterFloatAttributes().withLabel("%")
        ));

        // Playback mode parameters
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(PLAYBACK_LAYER, 1),
            "Layer Mode",
            true
        ));

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(PLAYBACK_RANDOM, 1),
            "Random Mode",
            false
        ));

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(RANDOM_OCTAVE, 1),
            "Random Octave",
            false
        ));

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(REVERSE, 1),
            "Reverse",
            false
        ));

        // Per-slot parameters
        for (int i = 0; i < NUM_SLOTS; ++i)
        {
            juce::String slotPrefix = "Slot " + juce::String(i + 1) + " ";

            // Volume
            params.push_back(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID(slotVolume(i), 1),
                slotPrefix + "Volume",
                juce::NormalisableRange<float>(-60.0f, 12.0f, 0.1f),
                0.0f,
                juce::AudioParameterFloatAttributes().withLabel("dB")
            ));

            // Pitch
            params.push_back(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID(slotPitch(i), 1),
                slotPrefix + "Pitch",
                juce::NormalisableRange<float>(-24.0f, 24.0f, 0.01f),
                0.0f,
                juce::AudioParameterFloatAttributes().withLabel("st")
            ));

            // ADSR Envelope
            params.push_back(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID(slotAttack(i), 1),
                slotPrefix + "Attack",
                juce::NormalisableRange<float>(0.0f, 10000.0f, 1.0f, 0.3f),
                10.0f,
                juce::AudioParameterFloatAttributes().withLabel("ms")
            ));

            params.push_back(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID(slotDecay(i), 1),
                slotPrefix + "Decay",
                juce::NormalisableRange<float>(0.0f, 10000.0f, 1.0f, 0.3f),
                100.0f,
                juce::AudioParameterFloatAttributes().withLabel("ms")
            ));

            params.push_back(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID(slotSustain(i), 1),
                slotPrefix + "Sustain",
                juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f),
                -6.0f,
                juce::AudioParameterFloatAttributes().withLabel("dB")
            ));

            params.push_back(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID(slotRelease(i), 1),
                slotPrefix + "Release",
                juce::NormalisableRange<float>(0.0f, 10000.0f, 1.0f, 0.3f),
                500.0f,
                juce::AudioParameterFloatAttributes().withLabel("ms")
            ));

            // In/Out points (0-100% of sample length)
            params.push_back(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID(slotInPoint(i), 1),
                slotPrefix + "In Point",
                juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
                0.0f,
                juce::AudioParameterFloatAttributes().withLabel("%")
            ));

            params.push_back(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID(slotOutPoint(i), 1),
                slotPrefix + "Out Point",
                juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
                100.0f,
                juce::AudioParameterFloatAttributes().withLabel("%")
            ));

            // Loop mode (for infinite playback)
            params.push_back(std::make_unique<juce::AudioParameterBool>(
                juce::ParameterID(slotLoop(i), 1),
                slotPrefix + "Loop",
                false
            ));

            // Filter parameters (Phase 2 - registered now for APVTS completeness)
            params.push_back(std::make_unique<juce::AudioParameterChoice>(
                juce::ParameterID(slotFilterType(i), 1),
                slotPrefix + "Filter Type",
                juce::StringArray{"Low Pass", "High Pass", "Band Pass"},
                0
            ));

            params.push_back(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID(slotFilterCutoff(i), 1),
                slotPrefix + "Filter Cutoff",
                juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f),
                10000.0f,
                juce::AudioParameterFloatAttributes().withLabel("Hz")
            ));

            params.push_back(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID(slotFilterResonance(i), 1),
                slotPrefix + "Filter Resonance",
                juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
                0.1f
            ));

            params.push_back(std::make_unique<juce::AudioParameterBool>(
                juce::ParameterID(slotFilterBypass(i), 1),
                slotPrefix + "Filter Bypass",
                true
            ));

            // LFO parameters (Phase 2)
            params.push_back(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID(slotLfoRate(i), 1),
                slotPrefix + "LFO Rate",
                juce::NormalisableRange<float>(0.01f, 20.0f, 0.01f, 0.5f),
                1.0f,
                juce::AudioParameterFloatAttributes().withLabel("Hz")
            ));

            params.push_back(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID(slotLfoDepth(i), 1),
                slotPrefix + "LFO Depth",
                juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
                0.0f
            ));

            params.push_back(std::make_unique<juce::AudioParameterChoice>(
                juce::ParameterID(slotLfoWaveform(i), 1),
                slotPrefix + "LFO Waveform",
                juce::StringArray{"Sine", "Triangle", "Square", "S&H"},
                0
            ));
        }

        // BBD Delay parameters (Phase 3)
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(DELAY_TIME, 1),
            "Delay Time",
            juce::NormalisableRange<float>(1.0f, 1000.0f, 1.0f, 0.4f),
            213.0f,
            juce::AudioParameterFloatAttributes().withLabel("ms")
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(DELAY_FEEDBACK, 1),
            "Delay Feedback",
            juce::NormalisableRange<float>(0.0f, 0.99f, 0.01f),
            0.5f
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(DELAY_MOD_DEPTH, 1),
            "Delay Mod Depth",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.0f
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(DELAY_MOD_RATE, 1),
            "Delay Mod Rate",
            juce::NormalisableRange<float>(0.01f, 10.0f, 0.01f, 0.5f),
            1.0f,
            juce::AudioParameterFloatAttributes().withLabel("Hz")
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(DELAY_TONE, 1),
            "Delay Tone",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.5f
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(DELAY_MIX, 1),
            "Delay Mix",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.3f
        ));

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(DELAY_BYPASS, 1),
            "Delay Bypass",
            true
        ));

        // BBD Chorus parameters
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(CHORUS_RATE, 1),
            "Chorus Rate",
            juce::NormalisableRange<float>(0.01f, 5.0f, 0.01f, 0.5f),
            0.5f,
            juce::AudioParameterFloatAttributes().withLabel("Hz")
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(CHORUS_DEPTH, 1),
            "Chorus Depth",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.5f
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(CHORUS_TONE, 1),
            "Chorus Tone",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.7f
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(CHORUS_MIX, 1),
            "Chorus Mix",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.3f
        ));

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(CHORUS_BYPASS, 1),
            "Chorus Bypass",
            true
        ));

        // Tape Saturation parameters
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(TAPE_DRIVE, 1),
            "Tape Drive",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.3f
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(TAPE_COMPRESSION, 1),
            "Tape Compression",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.3f
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(TAPE_TONE, 1),
            "Tape Tone",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.7f
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(TAPE_MIX, 1),
            "Tape Mix",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.5f
        ));

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(TAPE_BYPASS, 1),
            "Tape Bypass",
            true
        ));

        // Spectral Filter parameters
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(SPECTRAL_LOW, 1),
            "Spectral Low",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
            0.0f,
            juce::AudioParameterFloatAttributes().withLabel("dB")
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(SPECTRAL_MID, 1),
            "Spectral Mid",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
            0.0f,
            juce::AudioParameterFloatAttributes().withLabel("dB")
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(SPECTRAL_HIGH, 1),
            "Spectral High",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
            0.0f,
            juce::AudioParameterFloatAttributes().withLabel("dB")
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(SPECTRAL_SPREAD, 1),
            "Spectral Spread",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.0f
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(SPECTRAL_MIX, 1),
            "Spectral Mix",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            1.0f
        ));

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(SPECTRAL_BYPASS, 1),
            "Spectral Bypass",
            true
        ));

        return { params.begin(), params.end() };
    }
}
