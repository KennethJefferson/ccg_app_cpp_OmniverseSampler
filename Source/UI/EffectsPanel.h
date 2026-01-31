#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

class OmniverseAudioProcessor;

class DelaySection : public juce::Component
{
public:
    DelaySection(OmniverseAudioProcessor& processor);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void createSlider(juce::Slider& slider, const juce::String& paramId,
                      std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment);
    void createLabel(juce::Label& label, const juce::String& text);

    OmniverseAudioProcessor& processorRef;

    juce::Label sectionLabel;
    juce::Label titleLabel;

    juce::Slider timeSlider;
    juce::Slider feedbackSlider;
    juce::Slider modDepthSlider;
    juce::Slider modRateSlider;
    juce::Slider toneSlider;
    juce::Slider mixSlider;
    juce::TextButton powerButton;

    juce::Label timeLabel;
    juce::Label feedbackLabel;
    juce::Label modDepthLabel;
    juce::Label modRateLabel;
    juce::Label toneLabel;
    juce::Label mixLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> timeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modDepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modRateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> powerAttachment;
};

class ChorusSection : public juce::Component
{
public:
    ChorusSection(OmniverseAudioProcessor& processor);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void createSlider(juce::Slider& slider, const juce::String& paramId,
                      std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment);
    void createLabel(juce::Label& label, const juce::String& text);

    OmniverseAudioProcessor& processorRef;

    juce::Label sectionLabel;
    juce::Label titleLabel;

    juce::Slider rateSlider;
    juce::Slider depthSlider;
    juce::Slider toneSlider;
    juce::Slider mixSlider;
    juce::TextButton powerButton;

    juce::Label rateLabel;
    juce::Label depthLabel;
    juce::Label toneLabel;
    juce::Label mixLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> powerAttachment;
};

class TapeSaturationSection : public juce::Component
{
public:
    TapeSaturationSection(OmniverseAudioProcessor& processor);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void createSlider(juce::Slider& slider, const juce::String& paramId,
                      std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment);
    void createLabel(juce::Label& label, const juce::String& text);

    OmniverseAudioProcessor& processorRef;

    juce::Label sectionLabel;
    juce::Label titleLabel;

    juce::Slider driveSlider;
    juce::Slider compressionSlider;
    juce::Slider toneSlider;
    juce::Slider mixSlider;
    juce::TextButton powerButton;

    juce::Label driveLabel;
    juce::Label compressionLabel;
    juce::Label toneLabel;
    juce::Label mixLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compressionAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> powerAttachment;
};

class SpectralFilterSection : public juce::Component
{
public:
    SpectralFilterSection(OmniverseAudioProcessor& processor);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void createSlider(juce::Slider& slider, const juce::String& paramId,
                      std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment);
    void createLabel(juce::Label& label, const juce::String& text);

    OmniverseAudioProcessor& processorRef;

    juce::Label sectionLabel;
    juce::Label titleLabel;

    juce::Slider lowSlider;
    juce::Slider midSlider;
    juce::Slider highSlider;
    juce::Slider spreadSlider;
    juce::Slider mixSlider;
    juce::TextButton powerButton;

    juce::Label lowLabel;
    juce::Label midLabel;
    juce::Label highLabel;
    juce::Label spreadLabel;
    juce::Label mixLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> midAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> spreadAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> powerAttachment;
};

class EffectsPanel : public juce::Component
{
public:
    EffectsPanel(OmniverseAudioProcessor& processor);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    OmniverseAudioProcessor& processorRef;

    // Effect sections
    std::unique_ptr<DelaySection> delaySection;
    std::unique_ptr<ChorusSection> chorusSection;
    std::unique_ptr<TapeSaturationSection> tapeSection;
    std::unique_ptr<SpectralFilterSection> spectralSection;
};
