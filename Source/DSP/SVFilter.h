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
        // Sanitize input
        if (!std::isfinite(input))
            return 0.0f;

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

        // Protect filter state from corruption
        if (!std::isfinite(ic1eq)) ic1eq = 0.0f;
        if (!std::isfinite(ic2eq)) ic2eq = 0.0f;

        // Output based on filter type
        float output;
        switch (type)
        {
            case Type::LowPass:
                output = v2;
                break;
            case Type::HighPass:
                output = input - k * v1 - v2;
                break;
            case Type::BandPass:
                output = v1;
                break;
            default:
                output = v2;
                break;
        }

        return std::isfinite(output) ? output : input;
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
