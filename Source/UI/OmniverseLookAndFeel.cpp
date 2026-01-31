#include "OmniverseLookAndFeel.h"

OmniverseLookAndFeel::OmniverseLookAndFeel()
{
    // Set default colours
    setColour(juce::ResizableWindow::backgroundColourId, backgroundColour);
    setColour(juce::Label::textColourId, textColour);
    setColour(juce::TextButton::buttonColourId, surfaceColour);
    setColour(juce::TextButton::textColourOffId, textColour);
    setColour(juce::Slider::backgroundColourId, surfaceColour);
    setColour(juce::Slider::trackColourId, accentColour);
    setColour(juce::Slider::thumbColourId, textColour);
}

void OmniverseLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                             float sliderPos, float /*minSliderPos*/, float /*maxSliderPos*/,
                                             juce::Slider::SliderStyle style, juce::Slider& slider)
{
    auto bounds = juce::Rectangle<float>(static_cast<float>(x), static_cast<float>(y),
                                          static_cast<float>(width), static_cast<float>(height));

    // Background
    g.setColour(surfaceColour);
    g.fillRoundedRectangle(bounds, 3.0f);

    // Track (filled portion)
    if (style == juce::Slider::LinearBar || style == juce::Slider::LinearBarVertical)
    {
        auto fillBounds = bounds;

        if (style == juce::Slider::LinearBarVertical)
        {
            float fillHeight = bounds.getHeight() * (1.0f - sliderPos / static_cast<float>(width));
            fillBounds = bounds.removeFromBottom(fillHeight);
        }
        else
        {
            float fillWidth = sliderPos - static_cast<float>(x);
            fillBounds.setWidth(fillWidth);
        }

        g.setColour(accentColour.withAlpha(0.6f));
        g.fillRoundedRectangle(fillBounds, 3.0f);
    }

    // Border
    g.setColour(juce::Colour(0xFF3A3A3A));
    g.drawRoundedRectangle(bounds, 3.0f, 1.0f);

    // Value text (handled by slider's text box)
    juce::ignoreUnused(slider);
}

void OmniverseLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                             bool shouldDrawButtonAsHighlighted,
                                             bool /*shouldDrawButtonAsDown*/)
{
    auto bounds = button.getLocalBounds().toFloat();

    // Draw toggle indicator
    auto indicatorBounds = bounds.removeFromLeft(16.0f).reduced(2.0f);
    indicatorBounds = indicatorBounds.withSizeKeepingCentre(12.0f, 12.0f);

    g.setColour(surfaceColour);
    g.fillRoundedRectangle(indicatorBounds, 3.0f);

    if (button.getToggleState())
    {
        // Hot pink fill when active
        g.setColour(secondaryColour);
        g.fillRoundedRectangle(indicatorBounds.reduced(2.0f), 2.0f);
    }

    // Violet border
    g.setColour(button.getToggleState() ? accentColour : juce::Colour(0xFF3A3A3A));
    g.drawRoundedRectangle(indicatorBounds, 3.0f, 1.0f);

    // Draw text
    g.setColour(button.getToggleState() ? textColour : dimTextColour);
    if (shouldDrawButtonAsHighlighted)
        g.setColour(textColour);

    g.setFont(getBodyFont(18.0f));
    g.drawText(button.getButtonText(), bounds.reduced(4, 0), juce::Justification::centredLeft);
}

void OmniverseLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                                 const juce::Colour& /*backgroundColour*/,
                                                 bool shouldDrawButtonAsHighlighted,
                                                 bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();

    juce::Colour bgColour = surfaceColour;

    if (shouldDrawButtonAsDown)
        bgColour = accentColour.withAlpha(0.4f);
    else if (shouldDrawButtonAsHighlighted)
        bgColour = surfaceColour.brighter(0.1f);

    g.setColour(bgColour);
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(juce::Colour(0xFF3A3A3A));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
}

juce::Font OmniverseLookAndFeel::getTextButtonFont(juce::TextButton&, int /*buttonHeight*/)
{
    return getBodyFont(18.0f);
}

juce::Font OmniverseLookAndFeel::getLabelFont(juce::Label& /*label*/)
{
    return getBodyFont(16.0f);
}

juce::Font OmniverseLookAndFeel::getComboBoxFont(juce::ComboBox&)
{
    return getBodyFont(18.0f);
}

juce::Font OmniverseLookAndFeel::getPopupMenuFont()
{
    return getBodyFont(18.0f);
}

juce::Font OmniverseLookAndFeel::getTitleFont(float height)
{
    return juce::Font(fontName, height, juce::Font::plain);
}

juce::Font OmniverseLookAndFeel::getBodyFont(float height)
{
    return juce::Font(fontName, height, juce::Font::plain);
}

juce::Font OmniverseLookAndFeel::getBoldFont(float height)
{
    return juce::Font(fontName, height, juce::Font::bold);
}
