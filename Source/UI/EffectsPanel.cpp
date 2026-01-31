#include "EffectsPanel.h"
#include "../PluginProcessor.h"
#include "../Utils/Parameters.h"

// DelaySection implementation
DelaySection::DelaySection(OmniverseAudioProcessor& processor)
    : processorRef(processor)
{
    auto& apvts = processorRef.getAPVTS();

    // Section label
    sectionLabel.setText("time", juce::dontSendNotification);
    sectionLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    sectionLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(sectionLabel);

    titleLabel.setText("bbd delay", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(11.0f));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xFF8B5CF6));
    addAndMakeVisible(titleLabel);

    // Time
    createSlider(timeSlider, Parameters::DELAY_TIME, timeAttachment);
    timeSlider.setTextValueSuffix(" ms");
    createLabel(timeLabel, "time");

    // Feedback
    createSlider(feedbackSlider, Parameters::DELAY_FEEDBACK, feedbackAttachment);
    createLabel(feedbackLabel, "feedback");

    // Mod Depth
    createSlider(modDepthSlider, Parameters::DELAY_MOD_DEPTH, modDepthAttachment);
    createLabel(modDepthLabel, "mod depth");

    // Mod Rate
    createSlider(modRateSlider, Parameters::DELAY_MOD_RATE, modRateAttachment);
    modRateSlider.setTextValueSuffix(" Hz");
    createLabel(modRateLabel, "mod rate");

    // Tone
    createSlider(toneSlider, Parameters::DELAY_TONE, toneAttachment);
    createLabel(toneLabel, "tone");

    // Mix
    createSlider(mixSlider, Parameters::DELAY_MIX, mixAttachment);
    createLabel(mixLabel, "mix");

    // Bypass
    bypassButton.setButtonText("bypass");
    bypassButton.setColour(juce::ToggleButton::textColourId, juce::Colours::grey);
    addAndMakeVisible(bypassButton);
    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, Parameters::DELAY_BYPASS, bypassButton);
}

void DelaySection::createSlider(juce::Slider& slider, const juce::String& paramId,
                                 std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment)
{
    slider.setSliderStyle(juce::Slider::LinearBar);
    slider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 55, 18);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    slider.setColour(juce::Slider::trackColourId, juce::Colour(0xFF3A3A3A));
    slider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xFF1A1A1A));
    addAndMakeVisible(slider);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getAPVTS(), paramId, slider);
}

void DelaySection::createLabel(juce::Label& label, const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::Font(10.0f));
    label.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(label);
}

void DelaySection::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour(juce::Colour(0xFF0F0F0F));
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(juce::Colour(0xFF2A2A2A));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 4.0f, 1.0f);
}

void DelaySection::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    // Header
    auto headerRow = bounds.removeFromTop(20);
    sectionLabel.setBounds(headerRow.removeFromLeft(50));
    titleLabel.setBounds(headerRow);
    bounds.removeFromTop(5);

    // 2 columns layout
    int colWidth = bounds.getWidth() / 2;
    int rowHeight = 32;

    auto leftCol = bounds.removeFromLeft(colWidth).reduced(2, 0);
    auto rightCol = bounds.reduced(2, 0);

    // Left column: time, mod depth, tone
    auto timeRow = leftCol.removeFromTop(rowHeight);
    timeLabel.setBounds(timeRow.removeFromTop(12));
    timeSlider.setBounds(timeRow);

    auto modDepthRow = leftCol.removeFromTop(rowHeight);
    modDepthLabel.setBounds(modDepthRow.removeFromTop(12));
    modDepthSlider.setBounds(modDepthRow);

    auto toneRow = leftCol.removeFromTop(rowHeight);
    toneLabel.setBounds(toneRow.removeFromTop(12));
    toneSlider.setBounds(toneRow);

    // Right column: feedback, mod rate, mix
    auto feedbackRow = rightCol.removeFromTop(rowHeight);
    feedbackLabel.setBounds(feedbackRow.removeFromTop(12));
    feedbackSlider.setBounds(feedbackRow);

    auto modRateRow = rightCol.removeFromTop(rowHeight);
    modRateLabel.setBounds(modRateRow.removeFromTop(12));
    modRateSlider.setBounds(modRateRow);

    auto mixRow = rightCol.removeFromTop(rowHeight);
    mixLabel.setBounds(mixRow.removeFromTop(12));
    mixSlider.setBounds(mixRow);

    // Bypass at bottom
    bypassButton.setBounds(leftCol.removeFromTop(25));
}

// ChorusSection implementation
ChorusSection::ChorusSection(OmniverseAudioProcessor& processor)
    : processorRef(processor)
{
    auto& apvts = processorRef.getAPVTS();

    // Section label
    sectionLabel.setText("movement", juce::dontSendNotification);
    sectionLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    sectionLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(sectionLabel);

    titleLabel.setText("bbd chorus", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(11.0f));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xFF8B5CF6));
    addAndMakeVisible(titleLabel);

    // Rate
    createSlider(rateSlider, Parameters::CHORUS_RATE, rateAttachment);
    rateSlider.setTextValueSuffix(" Hz");
    createLabel(rateLabel, "rate");

    // Depth
    createSlider(depthSlider, Parameters::CHORUS_DEPTH, depthAttachment);
    createLabel(depthLabel, "depth");

    // Tone
    createSlider(toneSlider, Parameters::CHORUS_TONE, toneAttachment);
    createLabel(toneLabel, "tone");

    // Mix
    createSlider(mixSlider, Parameters::CHORUS_MIX, mixAttachment);
    createLabel(mixLabel, "mix");

    // Bypass
    bypassButton.setButtonText("bypass");
    bypassButton.setColour(juce::ToggleButton::textColourId, juce::Colours::grey);
    addAndMakeVisible(bypassButton);
    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, Parameters::CHORUS_BYPASS, bypassButton);
}

void ChorusSection::createSlider(juce::Slider& slider, const juce::String& paramId,
                                  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment)
{
    slider.setSliderStyle(juce::Slider::LinearBar);
    slider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 55, 18);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    slider.setColour(juce::Slider::trackColourId, juce::Colour(0xFF3A3A3A));
    slider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xFF1A1A1A));
    addAndMakeVisible(slider);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getAPVTS(), paramId, slider);
}

void ChorusSection::createLabel(juce::Label& label, const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::Font(10.0f));
    label.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(label);
}

void ChorusSection::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour(juce::Colour(0xFF0F0F0F));
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(juce::Colour(0xFF2A2A2A));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 4.0f, 1.0f);
}

void ChorusSection::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    // Header
    auto headerRow = bounds.removeFromTop(20);
    sectionLabel.setBounds(headerRow.removeFromLeft(80));
    titleLabel.setBounds(headerRow);
    bounds.removeFromTop(5);

    // 2 columns layout
    int colWidth = bounds.getWidth() / 2;
    int rowHeight = 32;

    auto leftCol = bounds.removeFromLeft(colWidth).reduced(2, 0);
    auto rightCol = bounds.reduced(2, 0);

    // Left column: rate, tone
    auto rateRow = leftCol.removeFromTop(rowHeight);
    rateLabel.setBounds(rateRow.removeFromTop(12));
    rateSlider.setBounds(rateRow);

    auto toneRow = leftCol.removeFromTop(rowHeight);
    toneLabel.setBounds(toneRow.removeFromTop(12));
    toneSlider.setBounds(toneRow);

    // Right column: depth, mix
    auto depthRow = rightCol.removeFromTop(rowHeight);
    depthLabel.setBounds(depthRow.removeFromTop(12));
    depthSlider.setBounds(depthRow);

    auto mixRow = rightCol.removeFromTop(rowHeight);
    mixLabel.setBounds(mixRow.removeFromTop(12));
    mixSlider.setBounds(mixRow);

    // Bypass at bottom
    bypassButton.setBounds(leftCol.removeFromTop(25));
}

// TapeSaturationSection implementation
TapeSaturationSection::TapeSaturationSection(OmniverseAudioProcessor& processor)
    : processorRef(processor)
{
    auto& apvts = processorRef.getAPVTS();

    // Section label
    sectionLabel.setText("color", juce::dontSendNotification);
    sectionLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    sectionLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(sectionLabel);

    titleLabel.setText("tape saturation", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(11.0f));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xFF8B5CF6));
    addAndMakeVisible(titleLabel);

    // Drive
    createSlider(driveSlider, Parameters::TAPE_DRIVE, driveAttachment);
    createLabel(driveLabel, "drive");

    // Compression
    createSlider(compressionSlider, Parameters::TAPE_COMPRESSION, compressionAttachment);
    createLabel(compressionLabel, "compression");

    // Tone
    createSlider(toneSlider, Parameters::TAPE_TONE, toneAttachment);
    createLabel(toneLabel, "tone");

    // Mix
    createSlider(mixSlider, Parameters::TAPE_MIX, mixAttachment);
    createLabel(mixLabel, "mix");

    // Bypass
    bypassButton.setButtonText("bypass");
    bypassButton.setColour(juce::ToggleButton::textColourId, juce::Colours::grey);
    addAndMakeVisible(bypassButton);
    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, Parameters::TAPE_BYPASS, bypassButton);
}

void TapeSaturationSection::createSlider(juce::Slider& slider, const juce::String& paramId,
                                          std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment)
{
    slider.setSliderStyle(juce::Slider::LinearBar);
    slider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 55, 18);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    slider.setColour(juce::Slider::trackColourId, juce::Colour(0xFF3A3A3A));
    slider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xFF1A1A1A));
    addAndMakeVisible(slider);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getAPVTS(), paramId, slider);
}

void TapeSaturationSection::createLabel(juce::Label& label, const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::Font(10.0f));
    label.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(label);
}

void TapeSaturationSection::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour(juce::Colour(0xFF0F0F0F));
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(juce::Colour(0xFF2A2A2A));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 4.0f, 1.0f);
}

void TapeSaturationSection::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    // Header
    auto headerRow = bounds.removeFromTop(20);
    sectionLabel.setBounds(headerRow.removeFromLeft(50));
    titleLabel.setBounds(headerRow);
    bounds.removeFromTop(5);

    // 2 columns layout
    int colWidth = bounds.getWidth() / 2;
    int rowHeight = 32;

    auto leftCol = bounds.removeFromLeft(colWidth).reduced(2, 0);
    auto rightCol = bounds.reduced(2, 0);

    // Left column: drive, tone
    auto driveRow = leftCol.removeFromTop(rowHeight);
    driveLabel.setBounds(driveRow.removeFromTop(12));
    driveSlider.setBounds(driveRow);

    auto toneRow = leftCol.removeFromTop(rowHeight);
    toneLabel.setBounds(toneRow.removeFromTop(12));
    toneSlider.setBounds(toneRow);

    // Right column: compression, mix
    auto compressionRow = rightCol.removeFromTop(rowHeight);
    compressionLabel.setBounds(compressionRow.removeFromTop(12));
    compressionSlider.setBounds(compressionRow);

    auto mixRow = rightCol.removeFromTop(rowHeight);
    mixLabel.setBounds(mixRow.removeFromTop(12));
    mixSlider.setBounds(mixRow);

    // Bypass at bottom
    bypassButton.setBounds(leftCol.removeFromTop(25));
}

// SpectralFilterSection implementation
SpectralFilterSection::SpectralFilterSection(OmniverseAudioProcessor& processor)
    : processorRef(processor)
{
    auto& apvts = processorRef.getAPVTS();

    // Section label
    sectionLabel.setText("master", juce::dontSendNotification);
    sectionLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    sectionLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(sectionLabel);

    titleLabel.setText("spectral filter", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(11.0f));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xFF8B5CF6));
    addAndMakeVisible(titleLabel);

    // Low
    createSlider(lowSlider, Parameters::SPECTRAL_LOW, lowAttachment);
    lowSlider.setTextValueSuffix(" dB");
    createLabel(lowLabel, "low");

    // Mid
    createSlider(midSlider, Parameters::SPECTRAL_MID, midAttachment);
    midSlider.setTextValueSuffix(" dB");
    createLabel(midLabel, "mid");

    // High
    createSlider(highSlider, Parameters::SPECTRAL_HIGH, highAttachment);
    highSlider.setTextValueSuffix(" dB");
    createLabel(highLabel, "high");

    // Spread
    createSlider(spreadSlider, Parameters::SPECTRAL_SPREAD, spreadAttachment);
    createLabel(spreadLabel, "spread");

    // Mix
    createSlider(mixSlider, Parameters::SPECTRAL_MIX, mixAttachment);
    createLabel(mixLabel, "mix");

    // Bypass
    bypassButton.setButtonText("bypass");
    bypassButton.setColour(juce::ToggleButton::textColourId, juce::Colours::grey);
    addAndMakeVisible(bypassButton);
    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, Parameters::SPECTRAL_BYPASS, bypassButton);
}

void SpectralFilterSection::createSlider(juce::Slider& slider, const juce::String& paramId,
                                          std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment)
{
    slider.setSliderStyle(juce::Slider::LinearBar);
    slider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 55, 18);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    slider.setColour(juce::Slider::trackColourId, juce::Colour(0xFF3A3A3A));
    slider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xFF1A1A1A));
    addAndMakeVisible(slider);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getAPVTS(), paramId, slider);
}

void SpectralFilterSection::createLabel(juce::Label& label, const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::Font(10.0f));
    label.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(label);
}

void SpectralFilterSection::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour(juce::Colour(0xFF0F0F0F));
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(juce::Colour(0xFF2A2A2A));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 4.0f, 1.0f);
}

void SpectralFilterSection::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    // Header
    auto headerRow = bounds.removeFromTop(20);
    sectionLabel.setBounds(headerRow.removeFromLeft(60));
    titleLabel.setBounds(headerRow);
    bounds.removeFromTop(5);

    // 2 columns layout
    int colWidth = bounds.getWidth() / 2;
    int rowHeight = 28;

    auto leftCol = bounds.removeFromLeft(colWidth).reduced(2, 0);
    auto rightCol = bounds.reduced(2, 0);

    // Left column: low, mid, high
    auto lowRow = leftCol.removeFromTop(rowHeight);
    lowLabel.setBounds(lowRow.removeFromTop(11));
    lowSlider.setBounds(lowRow);

    auto midRow = leftCol.removeFromTop(rowHeight);
    midLabel.setBounds(midRow.removeFromTop(11));
    midSlider.setBounds(midRow);

    auto highRow = leftCol.removeFromTop(rowHeight);
    highLabel.setBounds(highRow.removeFromTop(11));
    highSlider.setBounds(highRow);

    // Right column: spread, mix
    auto spreadRow = rightCol.removeFromTop(rowHeight);
    spreadLabel.setBounds(spreadRow.removeFromTop(11));
    spreadSlider.setBounds(spreadRow);

    auto mixRow = rightCol.removeFromTop(rowHeight);
    mixLabel.setBounds(mixRow.removeFromTop(11));
    mixSlider.setBounds(mixRow);

    // Bypass at bottom
    bypassButton.setBounds(rightCol.removeFromTop(25));
}

// EffectsPanel implementation
EffectsPanel::EffectsPanel(OmniverseAudioProcessor& processor)
    : processorRef(processor)
{
    delaySection = std::make_unique<DelaySection>(processor);
    addAndMakeVisible(*delaySection);

    chorusSection = std::make_unique<ChorusSection>(processor);
    addAndMakeVisible(*chorusSection);

    tapeSection = std::make_unique<TapeSaturationSection>(processor);
    addAndMakeVisible(*tapeSection);

    spectralSection = std::make_unique<SpectralFilterSection>(processor);
    addAndMakeVisible(*spectralSection);
}

void EffectsPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF0A0A0A));
}

void EffectsPanel::resized()
{
    auto bounds = getLocalBounds().reduced(5);

    // 4 sections side by side
    int sectionWidth = bounds.getWidth() / 4;

    delaySection->setBounds(bounds.removeFromLeft(sectionWidth).reduced(3, 0));
    chorusSection->setBounds(bounds.removeFromLeft(sectionWidth).reduced(3, 0));
    tapeSection->setBounds(bounds.removeFromLeft(sectionWidth).reduced(3, 0));
    spectralSection->setBounds(bounds.reduced(3, 0));
}
