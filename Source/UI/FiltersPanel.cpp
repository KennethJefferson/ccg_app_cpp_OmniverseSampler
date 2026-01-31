#include "FiltersPanel.h"
#include "../PluginProcessor.h"
#include "../Utils/Parameters.h"

// SlotFilterPanel implementation
SlotFilterPanel::SlotFilterPanel(OmniverseAudioProcessor& processor, int index)
    : processorRef(processor)
    , slotIndex(index)
{
    auto& apvts = processorRef.getAPVTS();

    // Slot label
    slotLabel.setText("Slot " + juce::String(slotIndex + 1), juce::dontSendNotification);
    slotLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    slotLabel.setColour(juce::Label::textColourId, juce::Colour(0xFF8B5CF6));
    addAndMakeVisible(slotLabel);

    // Filter section
    filterLabel.setText("filter", juce::dontSendNotification);
    filterLabel.setFont(juce::Font(11.0f));
    filterLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(filterLabel);

    filterTypeBox.addItem("Low Pass", 1);
    filterTypeBox.addItem("High Pass", 2);
    filterTypeBox.addItem("Band Pass", 3);
    filterTypeBox.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xFF1A1A1A));
    filterTypeBox.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    filterTypeBox.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xFF3A3A3A));
    addAndMakeVisible(filterTypeBox);
    filterTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, Parameters::slotFilterType(slotIndex), filterTypeBox);

    createSlider(cutoffSlider, Parameters::slotFilterCutoff(slotIndex), cutoffAttachment);
    cutoffSlider.setTextValueSuffix(" Hz");
    cutoffSlider.setSkewFactorFromMidPoint(1000.0f);
    createLabel(cutoffLabel, "cutoff");

    createSlider(resonanceSlider, Parameters::slotFilterResonance(slotIndex), resonanceAttachment);
    createLabel(resonanceLabel, "resonance");

    // Power button (pink when ON/not bypassed, dim when OFF/bypassed)
    powerButton.setButtonText("ON");
    powerButton.setClickingTogglesState(true);
    powerButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFFFF006E));
    powerButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xFF2A2A2A));
    powerButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    powerButton.setColour(juce::TextButton::textColourOnId, juce::Colours::grey);
    addAndMakeVisible(powerButton);
    powerAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, Parameters::slotFilterBypass(slotIndex), powerButton);

    // LFO section
    lfoLabel.setText("lfo", juce::dontSendNotification);
    lfoLabel.setFont(juce::Font(11.0f));
    lfoLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(lfoLabel);

    createSlider(lfoRateSlider, Parameters::slotLfoRate(slotIndex), lfoRateAttachment);
    lfoRateSlider.setTextValueSuffix(" Hz");
    createLabel(lfoRateLabel, "rate");

    createSlider(lfoDepthSlider, Parameters::slotLfoDepth(slotIndex), lfoDepthAttachment);
    createLabel(lfoDepthLabel, "depth");

    lfoWaveformBox.addItem("Sine", 1);
    lfoWaveformBox.addItem("Triangle", 2);
    lfoWaveformBox.addItem("Square", 3);
    lfoWaveformBox.addItem("S&H", 4);
    lfoWaveformBox.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xFF1A1A1A));
    lfoWaveformBox.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    lfoWaveformBox.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xFF3A3A3A));
    addAndMakeVisible(lfoWaveformBox);
    lfoWaveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, Parameters::slotLfoWaveform(slotIndex), lfoWaveformBox);
}

void SlotFilterPanel::createSlider(juce::Slider& slider, const juce::String& paramId,
                                    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment)
{
    slider.setSliderStyle(juce::Slider::LinearBar);
    slider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 60, 20);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    slider.setColour(juce::Slider::trackColourId, juce::Colour(0xFF3A3A3A));
    slider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xFF1A1A1A));
    addAndMakeVisible(slider);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getAPVTS(), paramId, slider);
}

void SlotFilterPanel::createLabel(juce::Label& label, const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::Font(10.0f));
    label.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(label);
}

void SlotFilterPanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour(juce::Colour(0xFF0F0F0F));
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(juce::Colour(0xFF2A2A2A));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 4.0f, 1.0f);
}

void SlotFilterPanel::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    slotLabel.setBounds(bounds.removeFromTop(20));
    bounds.removeFromTop(5);

    // Filter section
    filterLabel.setBounds(bounds.removeFromTop(15));
    bounds.removeFromTop(3);

    filterTypeBox.setBounds(bounds.removeFromTop(22));
    bounds.removeFromTop(5);

    auto cutoffRow = bounds.removeFromTop(35);
    cutoffLabel.setBounds(cutoffRow.removeFromTop(12));
    cutoffSlider.setBounds(cutoffRow);
    bounds.removeFromTop(3);

    auto resRow = bounds.removeFromTop(35);
    resonanceLabel.setBounds(resRow.removeFromTop(12));
    resonanceSlider.setBounds(resRow);
    bounds.removeFromTop(3);

    powerButton.setBounds(bounds.removeFromTop(22).removeFromLeft(50));
    bounds.removeFromTop(8);

    // LFO section
    lfoLabel.setBounds(bounds.removeFromTop(15));
    bounds.removeFromTop(3);

    auto rateRow = bounds.removeFromTop(35);
    lfoRateLabel.setBounds(rateRow.removeFromTop(12));
    lfoRateSlider.setBounds(rateRow);
    bounds.removeFromTop(3);

    auto depthRow = bounds.removeFromTop(35);
    lfoDepthLabel.setBounds(depthRow.removeFromTop(12));
    lfoDepthSlider.setBounds(depthRow);
    bounds.removeFromTop(5);

    lfoWaveformBox.setBounds(bounds.removeFromTop(22));
}

// FiltersPanel implementation
FiltersPanel::FiltersPanel(OmniverseAudioProcessor& processor)
    : processorRef(processor)
{
    for (int i = 0; i < 5; ++i)
    {
        slotPanels[i] = std::make_unique<SlotFilterPanel>(processor, i);
        addAndMakeVisible(*slotPanels[i]);
    }
}

void FiltersPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF0A0A0A));
}

void FiltersPanel::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    int panelWidth = bounds.getWidth() / 5;

    for (int i = 0; i < 5; ++i)
    {
        slotPanels[i]->setBounds(bounds.removeFromLeft(panelWidth).reduced(3, 0));
    }
}
