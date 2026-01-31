#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "WaveformDisplay.h"

class OmniverseAudioProcessor;

class SlotPanel : public juce::Component,
                  public juce::FileDragAndDropTarget,
                  public juce::Timer
{
public:
    SlotPanel(OmniverseAudioProcessor& processor, int slotIndex);
    ~SlotPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // FileDragAndDropTarget
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;
    void fileDragEnter(const juce::StringArray& files, int x, int y) override;
    void fileDragExit(const juce::StringArray& files) override;

    // Timer for UI updates
    void timerCallback() override;

private:
    void createSlider(juce::Slider& slider, const juce::String& paramId,
                      std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment);
    void createLabel(juce::Label& label, const juce::String& text);

    OmniverseAudioProcessor& processorRef;
    int slotIndex;

    WaveformDisplay waveformDisplay;
    bool isDragOver = false;

    // Controls
    juce::Slider volumeSlider;
    juce::Slider pitchSlider;
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;
    juce::Slider inPointSlider;
    juce::Slider outPointSlider;

    juce::Label volumeLabel;
    juce::Label pitchLabel;
    juce::Label envelopeLabel;
    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label sustainLabel;
    juce::Label releaseLabel;
    juce::Label inOutLabel;
    juce::Label inPointLabel;
    juce::Label outPointLabel;

    // APVTS Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inPointAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outPointAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SlotPanel)
};
