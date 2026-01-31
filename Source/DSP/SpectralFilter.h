#pragma once

#include <cmath>
#include <algorithm>

class SpectralFilter
{
public:
    SpectralFilter() = default;

    void prepare(double newSampleRate, int /*maxBlockSize*/)
    {
        sampleRate = newSampleRate;
        resetFilters();
    }

    void reset()
    {
        resetFilters();
    }

    void setLowGain(float g) { lowGain = std::clamp(g, -12.0f, 12.0f); }
    void setMidGain(float g) { midGain = std::clamp(g, -12.0f, 12.0f); }
    void setHighGain(float g) { highGain = std::clamp(g, -12.0f, 12.0f); }
    void setSpread(float s) { spread = std::clamp(s, 0.0f, 1.0f); }
    void setMix(float m) { mix = std::clamp(m, 0.0f, 1.0f); }

    void processBlock(float* left, float* right, int numSamples)
    {
        // Convert dB gains to linear
        float lowLinear = std::pow(10.0f, lowGain / 20.0f);
        float midLinear = std::pow(10.0f, midGain / 20.0f);
        float highLinear = std::pow(10.0f, highGain / 20.0f);

        for (int i = 0; i < numSamples; ++i)
        {
            float dryL = left[i];
            float dryR = right[i];

            // === SPLIT INTO 3 BANDS ===

            // Low band: below 300Hz
            float lowL = lowPassL1(left[i], 300.0f);
            float lowR = lowPassR1(right[i], 300.0f);

            // High band: above 4kHz
            float highL = highPassL(left[i], 4000.0f);
            float highR = highPassR(right[i], 4000.0f);

            // Mid band: 300Hz - 4kHz (what's left)
            float midL = left[i] - lowL - highL;
            float midR = right[i] - lowR - highR;

            // === APPLY GAINS ===
            lowL *= lowLinear;
            lowR *= lowLinear;
            midL *= midLinear;
            midR *= midLinear;
            highL *= highLinear;
            highR *= highLinear;

            // === SPECTRAL SPREAD (stereo widening per band) ===
            if (spread > 0.0f)
            {
                // Apply different stereo widths to each band
                // Low: mono, Mid: normal, High: wide
                float lowMono = (lowL + lowR) * 0.5f;
                lowL = lowMono + (lowL - lowMono) * (1.0f - spread * 0.5f);
                lowR = lowMono + (lowR - lowMono) * (1.0f - spread * 0.5f);

                // Mid stays as-is (or slight widening)
                float midMid = (midL + midR) * 0.5f;
                float midSide = (midL - midR) * 0.5f;
                midSide *= (1.0f + spread * 0.3f);
                midL = midMid + midSide;
                midR = midMid - midSide;

                // High: enhanced stereo
                float highMid = (highL + highR) * 0.5f;
                float highSide = (highL - highR) * 0.5f;
                highSide *= (1.0f + spread);
                highL = highMid + highSide;
                highR = highMid - highSide;
            }

            // === RECOMBINE BANDS ===
            float wetL = lowL + midL + highL;
            float wetR = lowR + midR + highR;

            // === SOFT LIMIT (prevent clipping from boosts) ===
            wetL = softLimit(wetL);
            wetR = softLimit(wetR);

            // === MIX ===
            left[i] = dryL * (1.0f - mix) + wetL * mix;
            right[i] = dryR * (1.0f - mix) + wetR * mix;
        }
    }

private:
    // === BAND-SPLIT FILTERS ===

    // Low-pass for low band extraction (2-pole)
    float lowPassL1(float input, float cutoffHz)
    {
        float omega = 2.0f * 3.14159265359f * cutoffHz / static_cast<float>(sampleRate);
        float g = omega / (1.0f + omega);

        lpfStateL1 = lpfStateL1 + g * (input - lpfStateL1);
        lpfStateL2 = lpfStateL2 + g * (lpfStateL1 - lpfStateL2);

        return lpfStateL2;
    }

    float lowPassR1(float input, float cutoffHz)
    {
        float omega = 2.0f * 3.14159265359f * cutoffHz / static_cast<float>(sampleRate);
        float g = omega / (1.0f + omega);

        lpfStateR1 = lpfStateR1 + g * (input - lpfStateR1);
        lpfStateR2 = lpfStateR2 + g * (lpfStateR1 - lpfStateR2);

        return lpfStateR2;
    }

    // High-pass for high band extraction (2-pole)
    float highPassL(float input, float cutoffHz)
    {
        float omega = 2.0f * 3.14159265359f * cutoffHz / static_cast<float>(sampleRate);
        float g = omega / (1.0f + omega);

        hpfStateL1 = hpfStateL1 + g * (input - hpfStateL1);
        hpfStateL2 = hpfStateL2 + g * (hpfStateL1 - hpfStateL2);

        // High-pass = input - low-pass
        return input - hpfStateL2;
    }

    float highPassR(float input, float cutoffHz)
    {
        float omega = 2.0f * 3.14159265359f * cutoffHz / static_cast<float>(sampleRate);
        float g = omega / (1.0f + omega);

        hpfStateR1 = hpfStateR1 + g * (input - hpfStateR1);
        hpfStateR2 = hpfStateR2 + g * (hpfStateR1 - hpfStateR2);

        return input - hpfStateR2;
    }

    // === SOFT LIMITER ===
    float softLimit(float x)
    {
        if (std::abs(x) > 1.0f)
        {
            return std::tanh(x);
        }
        return x;
    }

    void resetFilters()
    {
        lpfStateL1 = 0.0f;
        lpfStateL2 = 0.0f;
        lpfStateR1 = 0.0f;
        lpfStateR2 = 0.0f;
        hpfStateL1 = 0.0f;
        hpfStateL2 = 0.0f;
        hpfStateR1 = 0.0f;
        hpfStateR2 = 0.0f;
    }

    // Parameters
    float lowGain = 0.0f;   // dB
    float midGain = 0.0f;   // dB
    float highGain = 0.0f;  // dB
    float spread = 0.0f;    // Spectral stereo spread
    float mix = 1.0f;       // Wet/dry mix

    // State
    double sampleRate = 44100.0;

    // Low-pass filter states (for low band)
    float lpfStateL1 = 0.0f;
    float lpfStateL2 = 0.0f;
    float lpfStateR1 = 0.0f;
    float lpfStateR2 = 0.0f;

    // High-pass filter states (for high band)
    float hpfStateL1 = 0.0f;
    float hpfStateL2 = 0.0f;
    float hpfStateR1 = 0.0f;
    float hpfStateR2 = 0.0f;
};
