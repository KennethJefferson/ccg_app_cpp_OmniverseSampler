#pragma once

#include <cmath>
#include <random>

class LFO
{
public:
    enum class Waveform
    {
        Sine,
        Triangle,
        Square,
        SampleAndHold
    };

    LFO() : randomEngine(std::random_device{}()) {}

    void prepare(double sampleRate)
    {
        this->sampleRate = sampleRate;
        reset();
    }

    void reset()
    {
        phase = 0.0;
        holdValue = 0.0f;
        lastPhase = 0.0;
    }

    void setRate(float rateHz) { rate = rateHz; }
    void setWaveform(Waveform newWaveform) { waveform = newWaveform; }

    float process()
    {
        float output = 0.0f;

        switch (waveform)
        {
            case Waveform::Sine:
                output = std::sin(2.0 * 3.14159265359 * phase);
                break;

            case Waveform::Triangle:
                // Triangle: ramp from -1 to 1 in first half, 1 to -1 in second half
                if (phase < 0.5)
                    output = static_cast<float>(4.0 * phase - 1.0);
                else
                    output = static_cast<float>(3.0 - 4.0 * phase);
                break;

            case Waveform::Square:
                output = phase < 0.5 ? 1.0f : -1.0f;
                break;

            case Waveform::SampleAndHold:
                // Update hold value at phase wrap
                if (phase < lastPhase)
                {
                    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
                    holdValue = dist(randomEngine);
                }
                output = holdValue;
                break;
        }

        lastPhase = phase;

        // Advance phase
        double phaseIncrement = rate / sampleRate;
        phase += phaseIncrement;
        if (phase >= 1.0)
            phase -= 1.0;

        return output;
    }

    // Get current value without advancing (for control rate processing)
    float getCurrentValue() const
    {
        switch (waveform)
        {
            case Waveform::Sine:
                return static_cast<float>(std::sin(2.0 * 3.14159265359 * phase));
            case Waveform::Triangle:
                if (phase < 0.5)
                    return static_cast<float>(4.0 * phase - 1.0);
                else
                    return static_cast<float>(3.0 - 4.0 * phase);
            case Waveform::Square:
                return phase < 0.5 ? 1.0f : -1.0f;
            case Waveform::SampleAndHold:
                return holdValue;
            default:
                return 0.0f;
        }
    }

private:
    double sampleRate = 44100.0;
    double phase = 0.0;
    double lastPhase = 0.0;
    float rate = 1.0f;
    Waveform waveform = Waveform::Sine;
    float holdValue = 0.0f;
    std::mt19937 randomEngine;
};
