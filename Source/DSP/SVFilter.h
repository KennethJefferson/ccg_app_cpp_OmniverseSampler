#pragma once

#include <cmath>

class SVFilter
{
public:
    enum class Type
    {
        LowPass,
        HighPass,
        BandPass
    };

    SVFilter() = default;

    void prepare(double sampleRate)
    {
        this->sampleRate = sampleRate;
        reset();
    }

    void reset()
    {
        ic1eq = 0.0f;
        ic2eq = 0.0f;
    }

    void setType(Type newType) { type = newType; }
    void setCutoff(float frequencyHz) { cutoffHz = frequencyHz; }
    void setResonance(float res) { resonance = std::clamp(res, 0.0f, 1.0f); }

    float process(float input)
    {
        // Clamp cutoff to valid range
        float freq = std::clamp(cutoffHz, 20.0f, static_cast<float>(sampleRate * 0.49));

        // Calculate coefficients (TPT/Trapezoidal SVF)
        float g = std::tan(3.14159265359f * freq / static_cast<float>(sampleRate));
        float k = 2.0f - 2.0f * resonance; // Q = 1/(2-2*res), so k = 2*(1-res) for stability

        // Ensure k doesn't go too low (prevents self-oscillation issues)
        k = std::max(k, 0.1f);

        float a1 = 1.0f / (1.0f + g * (g + k));
        float a2 = g * a1;
        float a3 = g * a2;

        // Process
        float v3 = input - ic2eq;
        float v1 = a1 * ic1eq + a2 * v3;
        float v2 = ic2eq + a2 * ic1eq + a3 * v3;

        // Update state
        ic1eq = 2.0f * v1 - ic1eq;
        ic2eq = 2.0f * v2 - ic2eq;

        // Output based on filter type
        switch (type)
        {
            case Type::LowPass:
                return v2;
            case Type::HighPass:
                return input - k * v1 - v2;
            case Type::BandPass:
                return v1;
            default:
                return v2;
        }
    }

private:
    double sampleRate = 44100.0;
    Type type = Type::LowPass;
    float cutoffHz = 10000.0f;
    float resonance = 0.1f;

    // State variables
    float ic1eq = 0.0f;
    float ic2eq = 0.0f;
};
