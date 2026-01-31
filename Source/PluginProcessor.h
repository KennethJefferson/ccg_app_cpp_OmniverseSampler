#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "Sampler/OmniverseSampler.h"
#include "Utils/Parameters.h"
#include "DSP/BBDDelay.h"
#include "DSP/BBDChorus.h"
#include "DSP/TapeSaturation.h"
#include "DSP/SpectralFilter.h"

class OmniverseAudioProcessor : public juce::AudioProcessor
{
public:
    OmniverseAudioProcessor();
    ~OmniverseAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    OmniverseSampler& getSampler() { return sampler; }

    bool loadSampleIntoSlot(int slotIndex, const juce::File& file);

private:
    void updateDelayParameters();
    void updateChorusParameters();
    void updateTapeParameters();
    void updateSpectralParameters();

    juce::AudioProcessorValueTreeState apvts;
    OmniverseSampler sampler;

    // Global effects
    BBDDelay bbdDelay;
    BBDChorus bbdChorus;
    TapeSaturation tapeSaturation;
    SpectralFilter spectralFilter;

    juce::AudioFormatManager formatManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OmniverseAudioProcessor)
};
