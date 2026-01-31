#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"
#include "UI/SlotPanel.h"
#include "UI/FiltersPanel.h"
#include "UI/EffectsPanel.h"
#include "UI/OmniverseLookAndFeel.h"

class OmniverseAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit OmniverseAudioProcessorEditor(OmniverseAudioProcessor&);
    ~OmniverseAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    OmniverseAudioProcessor& processorRef;
    OmniverseLookAndFeel lookAndFeel;

    // Tab state
    enum class Tab { Filters, Effects };
    Tab currentTab = Tab::Effects;

    // Header components
    juce::Label titleLabel;
    juce::Label versionLabel;
    juce::TextButton filtersTab;
    juce::TextButton effectsTab;

    // Master controls
    juce::Slider masterVolumeSlider;
    juce::Slider stereoWidthSlider;
    juce::Label masterVolumeLabel;
    juce::Label stereoWidthLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterVolumeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> stereoWidthAttachment;

    // Playback mode controls
    juce::ToggleButton layerButton;
    juce::ToggleButton randomButton;
    juce::ToggleButton randomOctaveButton;
    juce::ToggleButton reverseButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> layerAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> randomAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> randomOctaveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> reverseAttachment;

    // Slot panels (always visible at top)
    std::array<std::unique_ptr<SlotPanel>, 5> slotPanels;

    // Filters panel (bottom area, shown when filters tab active)
    std::unique_ptr<FiltersPanel> filtersPanel;

    // Effects panel (bottom area, shown when effects tab active)
    std::unique_ptr<EffectsPanel> effectsPanel;

    void setupHeader();
    void setupMasterControls();
    void setupPlaybackModes();
    void setupSlotPanels();
    void setupTabs();
    void switchToTab(Tab tab);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OmniverseAudioProcessorEditor)
};
