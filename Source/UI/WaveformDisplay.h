#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "../Sampler/SampleSlot.h"

class WaveformDisplay : public juce::Component
{
public:
    WaveformDisplay();

    void setSampleSlot(const SampleSlot* slot);
    void setInOutPoints(float inPercent, float outPercent);
    void setParameterReferences(juce::AudioProcessorValueTreeState* apvts, int slotIndex);

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseMove(const juce::MouseEvent& event) override;

private:
    enum class DragTarget { None, InPoint, OutPoint };

    float xPositionToPercent(float x) const;
    float percentToXPosition(float percent) const;
    DragTarget getHandleAtPosition(float x, float y) const;
    void updateCursor(DragTarget target);

    const SampleSlot* sampleSlot = nullptr;
    juce::AudioProcessorValueTreeState* apvtsRef = nullptr;
    int slotIdx = 0;

    float inPoint = 0.0f;   // 0-100%
    float outPoint = 100.0f; // 0-100%

    DragTarget currentDragTarget = DragTarget::None;
    static constexpr float HANDLE_HIT_WIDTH = 10.0f;  // Pixels for handle hit detection

    juce::Colour waveformColour { 0xFF8B5CF6 };    // Violet/Purple
    juce::Colour waveformAccent { 0xFFFF006E };    // Hot pink for highlights
    juce::Colour backgroundColour { 0xFF1A1A1A };
    juce::Colour inOutColour { 0xFFFF006E };       // Hot pink for in/out markers
    juce::Colour handleHoverColour { 0xFFFFFFFF }; // White when hovering
    juce::Colour dimmedColour { 0x80000000 };      // Semi-transparent black for outside region
};
