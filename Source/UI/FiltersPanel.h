#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

class OmniverseAudioProcessor;

class SlotFilterPanel : public juce::Component
{
public:
    SlotFilterPanel(OmniverseAudioProcessor& processor, int slotIndex);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void createSlider(juce::Slider& slider, const juce::String& paramId,
                      std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment);
    void createLabel(juce::Label& label, const juce::String& text);

    OmniverseAudioProcessor& processorRef;
    int slotIndex;

    juce::Label slotLabel;

    // Filter controls
    juce::Label filterLabel;
    juce::ComboBox filterTypeBox;
    juce::Slider cutoffSlider;
    juce::Slider resonanceSlider;
    juce::TextButton powerButton;

    juce::Label cutoffLabel;
    juce::Label resonanceLabel;

    // LFO controls
    juce::Label lfoLabel;
    juce::Slider lfoRateSlider;
    juce::Slider lfoDepthSlider;
    juce::ComboBox lfoWaveformBox;

    juce::Label lfoRateLabel;
    juce::Label lfoDepthLabel;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> resonanceAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> powerAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoRateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoDepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lfoWaveformAttachment;
};

class FiltersPanel : public juce::Component
{
public:
    FiltersPanel(OmniverseAudioProcessor& processor);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    OmniverseAudioProcessor& processorRef;
    std::array<std::unique_ptr<SlotFilterPanel>, 5> slotPanels;
};
