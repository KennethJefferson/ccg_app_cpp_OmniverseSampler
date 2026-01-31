#include "WaveformDisplay.h"
#include "../Utils/Parameters.h"

WaveformDisplay::WaveformDisplay()
{
    setOpaque(true);
}

void WaveformDisplay::setSampleSlot(const SampleSlot* slot)
{
    sampleSlot = slot;
    repaint();
}

void WaveformDisplay::setInOutPoints(float inPercent, float outPercent)
{
    // Only update if not currently dragging (to avoid fighting with user input)
    if (currentDragTarget == DragTarget::None)
    {
        inPoint = inPercent;
        outPoint = outPercent;
        repaint();
    }
}

void WaveformDisplay::setParameterReferences(juce::AudioProcessorValueTreeState* apvts, int slotIndex)
{
    apvtsRef = apvts;
    slotIdx = slotIndex;
}

float WaveformDisplay::xPositionToPercent(float x) const
{
    auto bounds = getLocalBounds().toFloat();
    float percent = ((x - bounds.getX()) / bounds.getWidth()) * 100.0f;
    return std::clamp(percent, 0.0f, 100.0f);
}

float WaveformDisplay::percentToXPosition(float percent) const
{
    auto bounds = getLocalBounds().toFloat();
    return bounds.getX() + (percent / 100.0f) * bounds.getWidth();
}

WaveformDisplay::DragTarget WaveformDisplay::getHandleAtPosition(float x, float /*y*/) const
{
    float inX = percentToXPosition(inPoint);
    float outX = percentToXPosition(outPoint);

    // Check if near in point handle
    if (std::abs(x - inX) <= HANDLE_HIT_WIDTH)
        return DragTarget::InPoint;

    // Check if near out point handle
    if (std::abs(x - outX) <= HANDLE_HIT_WIDTH)
        return DragTarget::OutPoint;

    return DragTarget::None;
}

void WaveformDisplay::updateCursor(DragTarget target)
{
    if (target != DragTarget::None)
        setMouseCursor(juce::MouseCursor::LeftRightResizeCursor);
    else
        setMouseCursor(juce::MouseCursor::NormalCursor);
}

void WaveformDisplay::mouseDown(const juce::MouseEvent& event)
{
    if (sampleSlot == nullptr || !sampleSlot->isLoaded())
        return;

    currentDragTarget = getHandleAtPosition(static_cast<float>(event.x), static_cast<float>(event.y));
}

void WaveformDisplay::mouseDrag(const juce::MouseEvent& event)
{
    if (currentDragTarget == DragTarget::None || apvtsRef == nullptr)
        return;

    float newPercent = xPositionToPercent(static_cast<float>(event.x));

    if (currentDragTarget == DragTarget::InPoint)
    {
        // Ensure in point doesn't go past out point
        newPercent = std::min(newPercent, outPoint - 1.0f);
        inPoint = std::max(0.0f, newPercent);

        // Update parameter
        if (auto* param = apvtsRef->getParameter(Parameters::slotInPoint(slotIdx)))
            param->setValueNotifyingHost(param->convertTo0to1(inPoint));
    }
    else if (currentDragTarget == DragTarget::OutPoint)
    {
        // Ensure out point doesn't go before in point
        newPercent = std::max(newPercent, inPoint + 1.0f);
        outPoint = std::min(100.0f, newPercent);

        // Update parameter
        if (auto* param = apvtsRef->getParameter(Parameters::slotOutPoint(slotIdx)))
            param->setValueNotifyingHost(param->convertTo0to1(outPoint));
    }

    repaint();
}

void WaveformDisplay::mouseUp(const juce::MouseEvent& /*event*/)
{
    currentDragTarget = DragTarget::None;
}

void WaveformDisplay::mouseMove(const juce::MouseEvent& event)
{
    if (sampleSlot == nullptr || !sampleSlot->isLoaded())
    {
        setMouseCursor(juce::MouseCursor::NormalCursor);
        return;
    }

    DragTarget hover = getHandleAtPosition(static_cast<float>(event.x), static_cast<float>(event.y));
    updateCursor(hover);
}

void WaveformDisplay::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.fillAll(backgroundColour);

    // Draw border - white/light gray for visibility
    g.setColour(juce::Colour(0xFF4A4A4A));
    g.drawRect(bounds, 1.5f);

    if (sampleSlot == nullptr || !sampleSlot->isLoaded())
    {
        // Draw placeholder text
        g.setColour(juce::Colours::grey);
        g.setFont(12.0f);
        g.drawText("Drop sample here", bounds, juce::Justification::centred);
        return;
    }

    const auto& thumbnailData = sampleSlot->getThumbnailData();
    if (thumbnailData.empty())
        return;

    // Draw waveform
    float centerY = bounds.getCentreY();
    float maxHeight = bounds.getHeight() * 0.45f;

    juce::Path waveformPath;
    bool pathStarted = false;

    int numPoints = static_cast<int>(thumbnailData.size());
    float xStep = bounds.getWidth() / static_cast<float>(numPoints);

    // Draw upper half
    for (int i = 0; i < numPoints; ++i)
    {
        float x = bounds.getX() + i * xStep;
        float y = centerY - (thumbnailData[i] * maxHeight);

        if (!pathStarted)
        {
            waveformPath.startNewSubPath(x, y);
            pathStarted = true;
        }
        else
        {
            waveformPath.lineTo(x, y);
        }
    }

    // Draw lower half (mirror)
    for (int i = numPoints - 1; i >= 0; --i)
    {
        float x = bounds.getX() + i * xStep;
        float y = centerY + (thumbnailData[i] * maxHeight);
        waveformPath.lineTo(x, y);
    }

    waveformPath.closeSubPath();

    // Fill with violet to pink gradient
    g.setGradientFill(juce::ColourGradient(
        waveformColour.withAlpha(0.8f), bounds.getCentreX(), centerY - maxHeight,
        waveformAccent.withAlpha(0.4f), bounds.getCentreX(), centerY + maxHeight,
        false
    ));
    g.fillPath(waveformPath);

    // Draw outline with hot pink for neon effect
    g.setColour(waveformAccent);
    g.strokePath(waveformPath, juce::PathStrokeType(1.5f));

    // === DRAW IN/OUT POINT OVERLAYS ===
    float inX = percentToXPosition(inPoint);
    float outX = percentToXPosition(outPoint);

    // Dim regions outside in/out range
    g.setColour(dimmedColour);
    if (inPoint > 0.0f)
    {
        g.fillRect(bounds.getX(), bounds.getY(), inX - bounds.getX(), bounds.getHeight());
    }
    if (outPoint < 100.0f)
    {
        g.fillRect(outX, bounds.getY(), bounds.getRight() - outX, bounds.getHeight());
    }

    // Determine if hovering over handles
    auto mousePos = getMouseXYRelative();
    bool hoverIn = (std::abs(static_cast<float>(mousePos.x) - inX) <= HANDLE_HIT_WIDTH);
    bool hoverOut = (std::abs(static_cast<float>(mousePos.x) - outX) <= HANDLE_HIT_WIDTH);

    // Draw in point marker and handle
    {
        bool isActive = hoverIn || currentDragTarget == DragTarget::InPoint;
        g.setColour(isActive ? handleHoverColour : inOutColour);

        // Vertical line
        g.drawLine(inX, bounds.getY(), inX, bounds.getBottom(), isActive ? 3.0f : 2.0f);

        // Handle at top - larger rectangle for easier grabbing
        float handleHeight = 12.0f;
        float handleWidth = 8.0f;
        juce::Rectangle<float> handleRect(inX - handleWidth / 2, bounds.getY(),
                                           handleWidth, handleHeight);
        g.fillRoundedRectangle(handleRect, 2.0f);

        // Draw "IN" label
        g.setFont(8.0f);
        g.drawText("IN", handleRect.translated(0, handleHeight + 2).withHeight(10),
                   juce::Justification::centred);
    }

    // Draw out point marker and handle
    {
        bool isActive = hoverOut || currentDragTarget == DragTarget::OutPoint;
        g.setColour(isActive ? handleHoverColour : inOutColour);

        // Vertical line
        g.drawLine(outX, bounds.getY(), outX, bounds.getBottom(), isActive ? 3.0f : 2.0f);

        // Handle at top - larger rectangle for easier grabbing
        float handleHeight = 12.0f;
        float handleWidth = 8.0f;
        juce::Rectangle<float> handleRect(outX - handleWidth / 2, bounds.getY(),
                                           handleWidth, handleHeight);
        g.fillRoundedRectangle(handleRect, 2.0f);

        // Draw "OUT" label
        g.setFont(8.0f);
        g.drawText("OUT", handleRect.translated(0, handleHeight + 2).withHeight(10),
                   juce::Justification::centred);
    }

    // Draw filename
    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.setFont(10.0f);
    g.drawText(sampleSlot->getFileName(),
               bounds.reduced(5, 5).removeFromBottom(15),
               juce::Justification::centredLeft);
}
