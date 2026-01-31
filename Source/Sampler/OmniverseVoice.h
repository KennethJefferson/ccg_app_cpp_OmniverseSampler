#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "SampleSlot.h"
#include "../DSP/SVFilter.h"
#include "../DSP/LFO.h"

class OmniverseVoice : public juce::SynthesiserVoice
{
public:
    OmniverseVoice();

    void setSlots(std::array<SampleSlot*, 5>* slots) { sampleSlots = slots; }
    void setAPVTS(juce::AudioProcessorValueTreeState* apvtsPtr) { apvts = apvtsPtr; }

    void prepareToPlay(double sampleRate, int samplesPerBlock);

    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;

    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                        int startSample, int numSamples) override;

    void setActiveSlots(const std::vector<int>& slots) { activeSlotIndices = slots; }
    void setReverse(bool reverse) { isReversed = reverse; }
    void setOctaveShift(int shift) { octaveShift = shift; }

private:
    struct SlotState
    {
        double samplePosition = 0.0;
        double envelopeTime = 0.0;
        float envelopeValue = 0.0f;
        bool inRelease = false;
        float releaseStartValue = 0.0f;
        double releaseTime = 0.0;
        bool isPlaying = false;
    };

    float calculateEnvelope(int slotIndex, SlotState& state, bool noteIsHeld);
    float getParameter(const juce::String& paramId);
    void updateFilterParameters(int slotIndex);

    std::array<SampleSlot*, 5>* sampleSlots = nullptr;
    juce::AudioProcessorValueTreeState* apvts = nullptr;

    std::array<SlotState, 5> slotStates;
    std::vector<int> activeSlotIndices;

    // Per-slot filters (stereo)
    std::array<SVFilter, 5> filtersL;
    std::array<SVFilter, 5> filtersR;

    // Per-slot LFOs
    std::array<LFO, 5> lfos;

    double currentSampleRate = 44100.0;
    int midiNote = 60;
    float noteVelocity = 1.0f;
    bool isReversed = false;
    int octaveShift = 0;

    // Control rate divider (update filters every N samples)
    int controlRateCounter = 0;
    static constexpr int CONTROL_RATE_DIVIDER = 32;

    static constexpr int ROOT_NOTE = 60;
};
