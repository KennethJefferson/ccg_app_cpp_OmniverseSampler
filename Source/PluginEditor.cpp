#include "PluginEditor.h"

OmniverseAudioProcessorEditor::OmniverseAudioProcessorEditor(OmniverseAudioProcessor& p)
    : AudioProcessorEditor(&p)
    , processorRef(p)
{
    setLookAndFeel(&lookAndFeel);

    setupHeader();
    setupMasterControls();
    setupPlaybackModes();
    setupSlotPanels();
    setupTabs();

    // Create filters panel
    filtersPanel = std::make_unique<FiltersPanel>(processorRef);
    addAndMakeVisible(*filtersPanel);

    // Create effects panel
    effectsPanel = std::make_unique<EffectsPanel>(processorRef);
    addAndMakeVisible(*effectsPanel);

    // Start on Effects tab
    switchToTab(Tab::Effects);

    setSize(1024, 700);
    setResizable(true, true);
    setResizeLimits(800, 600, 1920, 1080);
}

OmniverseAudioProcessorEditor::~OmniverseAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void OmniverseAudioProcessorEditor::setupHeader()
{
    titleLabel.setText("omniverse", juce::dontSendNotification);
    titleLabel.setFont(OmniverseLookAndFeel::getTitleFont(42.0f));
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);

    versionLabel.setText("v1.0.0", juce::dontSendNotification);
    versionLabel.setFont(OmniverseLookAndFeel::getBodyFont(10.0f));
    versionLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(versionLabel);
}

void OmniverseAudioProcessorEditor::setupTabs()
{
    filtersTab.setButtonText("filters");
    filtersTab.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    filtersTab.setColour(juce::TextButton::textColourOffId, juce::Colours::grey);
    filtersTab.onClick = [this]() { switchToTab(Tab::Filters); };
    addAndMakeVisible(filtersTab);

    effectsTab.setButtonText("effects");
    effectsTab.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    effectsTab.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    effectsTab.onClick = [this]() { switchToTab(Tab::Effects); };
    addAndMakeVisible(effectsTab);
}

void OmniverseAudioProcessorEditor::switchToTab(Tab tab)
{
    currentTab = tab;

    if (tab == Tab::Filters)
    {
        filtersTab.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        effectsTab.setColour(juce::TextButton::textColourOffId, juce::Colours::grey);
        filtersPanel->setVisible(true);
        effectsPanel->setVisible(false);
    }
    else
    {
        filtersTab.setColour(juce::TextButton::textColourOffId, juce::Colours::grey);
        effectsTab.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        filtersPanel->setVisible(false);
        effectsPanel->setVisible(true);
    }

    repaint();
}

void OmniverseAudioProcessorEditor::setupMasterControls()
{
    auto& apvts = processorRef.getAPVTS();

    masterVolumeSlider.setSliderStyle(juce::Slider::LinearBar);
    masterVolumeSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    masterVolumeSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    addAndMakeVisible(masterVolumeSlider);
    masterVolumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, Parameters::MASTER_VOLUME, masterVolumeSlider);

    masterVolumeLabel.setText("master volume", juce::dontSendNotification);
    masterVolumeLabel.setFont(juce::Font(10.0f));
    masterVolumeLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    masterVolumeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(masterVolumeLabel);

    stereoWidthSlider.setSliderStyle(juce::Slider::LinearBar);
    stereoWidthSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    stereoWidthSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    addAndMakeVisible(stereoWidthSlider);
    stereoWidthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, Parameters::STEREO_WIDTH, stereoWidthSlider);

    stereoWidthLabel.setText("stereo width", juce::dontSendNotification);
    stereoWidthLabel.setFont(juce::Font(10.0f));
    stereoWidthLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    stereoWidthLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(stereoWidthLabel);
}

void OmniverseAudioProcessorEditor::setupPlaybackModes()
{
    auto& apvts = processorRef.getAPVTS();

    layerButton.setButtonText("layer");
    layerButton.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible(layerButton);
    layerAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, Parameters::PLAYBACK_LAYER, layerButton);

    randomButton.setButtonText("random");
    randomButton.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible(randomButton);
    randomAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, Parameters::PLAYBACK_RANDOM, randomButton);

    randomOctaveButton.setButtonText("random octave");
    randomOctaveButton.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible(randomOctaveButton);
    randomOctaveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, Parameters::RANDOM_OCTAVE, randomOctaveButton);

    reverseButton.setButtonText("reverse");
    reverseButton.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible(reverseButton);
    reverseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, Parameters::REVERSE, reverseButton);
}

void OmniverseAudioProcessorEditor::setupSlotPanels()
{
    for (int i = 0; i < 5; ++i)
    {
        slotPanels[i] = std::make_unique<SlotPanel>(processorRef, i);
        addAndMakeVisible(*slotPanels[i]);
    }
}

void OmniverseAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF0A0A0A));

    // Draw subtle separator lines
    g.setColour(juce::Colour(0xFF2A2A2A));
    g.drawHorizontalLine(60, 0.0f, static_cast<float>(getWidth()));

    // Line between slots and bottom panel
    int slotBottomY = 60 + 10 + 280 + 10;
    g.drawHorizontalLine(slotBottomY, 0.0f, static_cast<float>(getWidth()));
}

void OmniverseAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    // Header area (60px)
    auto headerArea = bounds.removeFromTop(60);

    // Title and version stacked - larger title area
    auto titleArea = headerArea.removeFromLeft(200);
    titleLabel.setBounds(titleArea.getX() + 15, 5, 180, 45);
    versionLabel.setBounds(titleArea.getX() + 18, 48, 50, 12);

    // Tabs (with some spacing from title)
    headerArea.removeFromLeft(10);
    auto tabArea = headerArea.removeFromLeft(180);
    filtersTab.setBounds(tabArea.removeFromLeft(80).reduced(5, 15));
    effectsTab.setBounds(tabArea.removeFromLeft(80).reduced(5, 15));

    // Master controls on right side of header
    auto masterArea = headerArea.removeFromRight(200);
    auto volArea = masterArea.removeFromLeft(90);
    auto widthArea = masterArea;

    masterVolumeLabel.setBounds(volArea.getX(), 8, 80, 15);
    masterVolumeSlider.setBounds(volArea.getX(), 25, 80, 25);

    stereoWidthLabel.setBounds(widthArea.getX(), 8, 80, 15);
    stereoWidthSlider.setBounds(widthArea.getX(), 25, 80, 25);

    // Slot panels area (top section, ~280px)
    auto slotsArea = bounds.removeFromTop(280).reduced(10, 10);
    int slotWidth = slotsArea.getWidth() / 5;

    for (int i = 0; i < 5; ++i)
    {
        auto slotBounds = slotsArea.removeFromLeft(slotWidth).reduced(5, 0);
        slotPanels[i]->setBounds(slotBounds);
    }

    // Bottom area for tabs content and playback modes
    auto bottomArea = bounds;

    // Playback modes on left side
    auto playbackArea = bottomArea.removeFromLeft(140).reduced(10, 10);
    juce::Label playbackLabel;
    playbackLabel.setText("playback modes", juce::dontSendNotification);

    int buttonHeight = 25;
    int buttonY = playbackArea.getY();
    layerButton.setBounds(playbackArea.getX(), buttonY, 120, buttonHeight);
    buttonY += buttonHeight + 3;
    randomButton.setBounds(playbackArea.getX(), buttonY, 120, buttonHeight);
    buttonY += buttonHeight + 3;
    randomOctaveButton.setBounds(playbackArea.getX(), buttonY, 120, buttonHeight);
    buttonY += buttonHeight + 3;
    reverseButton.setBounds(playbackArea.getX(), buttonY, 120, buttonHeight);

    // Tab content area (right side)
    auto tabContentArea = bottomArea.reduced(5);
    filtersPanel->setBounds(tabContentArea);
    effectsPanel->setBounds(tabContentArea);
}
