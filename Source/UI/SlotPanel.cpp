#include "SlotPanel.h"
#include "../PluginProcessor.h"
#include "../Utils/Parameters.h"

SlotPanel::SlotPanel(OmniverseAudioProcessor& processor, int index)
    : processorRef(processor)
    , slotIndex(index)
{
    addAndMakeVisible(waveformDisplay);

    // Volume
    createSlider(volumeSlider, Parameters::slotVolume(slotIndex), volumeAttachment);
    volumeSlider.setTextValueSuffix(" dB");
    createLabel(volumeLabel, "volume");

    // Pitch
    createSlider(pitchSlider, Parameters::slotPitch(slotIndex), pitchAttachment);
    pitchSlider.setTextValueSuffix(" st");
    createLabel(pitchLabel, "pitch");

    // Envelope label
    createLabel(envelopeLabel, "envelope");
    envelopeLabel.setFont(juce::Font(12.0f, juce::Font::bold));

    // ADSR
    createSlider(attackSlider, Parameters::slotAttack(slotIndex), attackAttachment);
    attackSlider.setTextValueSuffix(" ms");
    createLabel(attackLabel, "attack");

    createSlider(decaySlider, Parameters::slotDecay(slotIndex), decayAttachment);
    decaySlider.setTextValueSuffix(" ms");
    createLabel(decayLabel, "decay");

    createSlider(sustainSlider, Parameters::slotSustain(slotIndex), sustainAttachment);
    sustainSlider.setTextValueSuffix(" dB");
    createLabel(sustainLabel, "sustain");

    createSlider(releaseSlider, Parameters::slotRelease(slotIndex), releaseAttachment);
    releaseSlider.setTextValueSuffix(" ms");
    createLabel(releaseLabel, "release");

    // In/Out label
    createLabel(inOutLabel, "in/out");
    inOutLabel.setFont(juce::Font(12.0f, juce::Font::bold));

    // In point
    createSlider(inPointSlider, Parameters::slotInPoint(slotIndex), inPointAttachment);
    inPointSlider.setTextValueSuffix("%");
    createLabel(inPointLabel, "in");

    // Out point
    createSlider(outPointSlider, Parameters::slotOutPoint(slotIndex), outPointAttachment);
    outPointSlider.setTextValueSuffix("%");
    createLabel(outPointLabel, "out");

    // Update waveform display
    waveformDisplay.setSampleSlot(processorRef.getSampler().getSlot(slotIndex));
    waveformDisplay.setParameterReferences(&processorRef.getAPVTS(), slotIndex);

    startTimerHz(10);
}

SlotPanel::~SlotPanel()
{
    stopTimer();
}

void SlotPanel::createSlider(juce::Slider& slider, const juce::String& paramId,
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

void SlotPanel::createLabel(juce::Label& label, const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::Font(11.0f));
    label.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(label);
}

void SlotPanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Background
    g.setColour(juce::Colour(0xFF0F0F0F));
    g.fillRoundedRectangle(bounds, 4.0f);

    // Drag highlight
    if (isDragOver)
    {
        g.setColour(juce::Colour(0xFFFF006E).withAlpha(0.3f));
        g.fillRoundedRectangle(bounds, 4.0f);

        g.setColour(juce::Colour(0xFFFF006E));
        g.drawRoundedRectangle(bounds.reduced(1), 4.0f, 2.0f);
    }
}

void SlotPanel::resized()
{
    auto bounds = getLocalBounds().reduced(6);

    // Waveform display at top (reduced height)
    waveformDisplay.setBounds(bounds.removeFromTop(80));
    bounds.removeFromTop(4);

    // In/Out section (below waveform)
    inOutLabel.setBounds(bounds.removeFromTop(14));
    bounds.removeFromTop(1);

    auto inOutRow = bounds.removeFromTop(26);
    int halfWidth = bounds.getWidth() / 2;
    auto inArea = inOutRow.removeFromLeft(halfWidth);
    auto outArea = inOutRow;

    inPointLabel.setBounds(inArea.removeFromTop(10));
    inPointSlider.setBounds(inArea.removeFromTop(14));

    outPointLabel.setBounds(outArea.removeFromTop(10));
    outPointSlider.setBounds(outArea.removeFromTop(14));

    bounds.removeFromTop(4);

    // Volume and pitch row
    auto volPitchRow = bounds.removeFromTop(28);
    auto volArea = volPitchRow.removeFromLeft(halfWidth);
    auto pitchArea = volPitchRow;

    volumeLabel.setBounds(volArea.removeFromTop(10));
    volumeSlider.setBounds(volArea.removeFromTop(16));

    pitchLabel.setBounds(pitchArea.removeFromTop(10));
    pitchSlider.setBounds(pitchArea.removeFromTop(16));

    bounds.removeFromTop(4);

    // Envelope section
    envelopeLabel.setBounds(bounds.removeFromTop(14));
    bounds.removeFromTop(1);

    // ADSR in 2x2 grid
    int rowHeight = 26;

    auto row1 = bounds.removeFromTop(rowHeight);
    auto attackArea = row1.removeFromLeft(halfWidth);
    auto decayArea = row1;

    attackLabel.setBounds(attackArea.removeFromTop(10));
    attackSlider.setBounds(attackArea.removeFromTop(14));

    decayLabel.setBounds(decayArea.removeFromTop(10));
    decaySlider.setBounds(decayArea.removeFromTop(14));

    auto row2 = bounds.removeFromTop(rowHeight);
    auto sustainArea = row2.removeFromLeft(halfWidth);
    auto releaseArea = row2;

    sustainLabel.setBounds(sustainArea.removeFromTop(10));
    sustainSlider.setBounds(sustainArea.removeFromTop(14));

    releaseLabel.setBounds(releaseArea.removeFromTop(10));
    releaseSlider.setBounds(releaseArea.removeFromTop(14));
}

bool SlotPanel::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (const auto& file : files)
    {
        if (file.endsWithIgnoreCase(".wav") ||
            file.endsWithIgnoreCase(".aif") ||
            file.endsWithIgnoreCase(".aiff") ||
            file.endsWithIgnoreCase(".mp3") ||
            file.endsWithIgnoreCase(".flac") ||
            file.endsWithIgnoreCase(".ogg"))
        {
            return true;
        }
    }
    return false;
}

void SlotPanel::filesDropped(const juce::StringArray& files, int /*x*/, int /*y*/)
{
    isDragOver = false;
    repaint();

    for (const auto& filePath : files)
    {
        juce::File file(filePath);
        if (file.existsAsFile())
        {
            if (processorRef.loadSampleIntoSlot(slotIndex, file))
            {
                waveformDisplay.setSampleSlot(processorRef.getSampler().getSlot(slotIndex));
                break;
            }
        }
    }
}

void SlotPanel::fileDragEnter(const juce::StringArray& /*files*/, int /*x*/, int /*y*/)
{
    isDragOver = true;
    repaint();
}

void SlotPanel::fileDragExit(const juce::StringArray& /*files*/)
{
    isDragOver = false;
    repaint();
}

void SlotPanel::timerCallback()
{
    // Refresh waveform if needed
    auto* slot = processorRef.getSampler().getSlot(slotIndex);
    if (slot != nullptr)
    {
        waveformDisplay.setSampleSlot(slot);

        // Update in/out point visualization
        float inPoint = processorRef.getAPVTS().getRawParameterValue(Parameters::slotInPoint(slotIndex))->load();
        float outPoint = processorRef.getAPVTS().getRawParameterValue(Parameters::slotOutPoint(slotIndex))->load();
        waveformDisplay.setInOutPoints(inPoint, outPoint);
    }
}
