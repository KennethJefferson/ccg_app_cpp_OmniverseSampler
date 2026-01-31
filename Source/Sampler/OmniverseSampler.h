#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "SampleSlot.h"
#include "OmniverseVoice.h"

class OmniverseSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int /*midiNoteNumber*/) override { return true; }
    bool appliesToChannel(int /*midiChannel*/) override { return true; }
};

class OmniverseSampler : public juce::Synthesiser
{
public:
    static constexpr int NUM_SLOTS = 5;
    static constexpr int NUM_VOICES = 16;

    OmniverseSampler();

    void setAPVTS(juce::AudioProcessorValueTreeState* apvtsPtr);

    SampleSlot* getSlot(int index);
    const SampleSlot* getSlot(int index) const;

    void noteOn(int midiChannel, int midiNoteNumber, float velocity) override;

private:
    std::vector<int> determineActiveSlots();
    int getOctaveShift();

    std::array<SampleSlot, NUM_SLOTS> slots;
    std::array<SampleSlot*, NUM_SLOTS> slotPointers;

    juce::AudioProcessorValueTreeState* apvts = nullptr;
    juce::Random random;

    int roundRobinIndex = 0;
};
