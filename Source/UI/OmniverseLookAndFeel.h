#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class OmniverseLookAndFeel : public juce::LookAndFeel_V4
{
public:
    OmniverseLookAndFeel();

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          juce::Slider::SliderStyle style, juce::Slider& slider) override;

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override;

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;

    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override;
    juce::Font getLabelFont(juce::Label& label) override;
    juce::Font getComboBoxFont(juce::ComboBox&) override;
    juce::Font getPopupMenuFont() override;

    // Get fonts for different purposes (sizes increased 50% from base)
    static juce::Font getTitleFont(float height = 48.0f);
    static juce::Font getBodyFont(float height = 16.0f);
    static juce::Font getBoldFont(float height = 18.0f);

    static inline const juce::String fontName = "TX-02";

private:
    juce::Colour accentColour { 0xFF8B5CF6 };      // Violet/Purple primary
    juce::Colour secondaryColour { 0xFFFF006E };   // Hot pink accent
    juce::Colour backgroundColour { 0xFF0A0A0A };
    juce::Colour surfaceColour { 0xFF1A1A1A };
    juce::Colour textColour { 0xFFFFFFFF };
    juce::Colour dimTextColour { 0xFF888888 };
};
