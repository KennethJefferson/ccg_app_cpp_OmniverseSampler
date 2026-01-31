#pragma once

#include <vector>
#include <cmath>
#include <algorithm>

class BBDChorus
{
public:
    BBDChorus() = default;

    void prepare(double newSampleRate, int /*maxBlockSize*/)
    {
        sampleRate = newSampleRate;

        // Max delay of 50ms for chorus
        int maxDelaySamples = static_cast<int>(sampleRate * 0.05);

        // 2 voices for stereo chorus
        delayBufferL.resize(maxDelaySamples, 0.0f);
        delayBufferR.resize(maxDelaySamples, 0.0f);

        // Initialize LFO phases - offset for stereo width
        lfoPhaseL = 0.0;
        lfoPhaseR = 0.25; // 90 degree offset for stereo

        resetFilters();
    }

    void reset()
    {
        std::fill(delayBufferL.begin(), delayBufferL.end(), 0.0f);
        std::fill(delayBufferR.begin(), delayBufferR.end(), 0.0f);
        writeIndexL = 0;
        writeIndexR = 0;
        lfoPhaseL = 0.0;
        lfoPhaseR = 0.25;
        resetFilters();
    }

    void setRate(float r) { rate = std::clamp(r, 0.01f, 10.0f); }
    void setDepth(float d) { depth = std::clamp(d, 0.0f, 1.0f); }
    void setMix(float m) { mix = std::clamp(m, 0.0f, 1.0f); }
    void setTone(float t) { tone = std::clamp(t, 0.0f, 1.0f); }

    void processBlock(float* left, float* right, int numSamples)
    {
        if (delayBufferL.empty() || delayBufferR.empty())
            return;

        for (int i = 0; i < numSamples; ++i)
        {
            // === LFO MODULATION ===
            float lfoL = static_cast<float>(std::sin(2.0 * 3.14159265359 * lfoPhaseL));
            float lfoR = static_cast<float>(std::sin(2.0 * 3.14159265359 * lfoPhaseR));

            lfoPhaseL += rate / sampleRate;
            lfoPhaseR += rate / sampleRate;
            if (lfoPhaseL >= 1.0) lfoPhaseL -= 1.0;
            if (lfoPhaseR >= 1.0) lfoPhaseR -= 1.0;

            // === DELAY TIME CALCULATION ===
            // Base delay: 7ms, modulation range: +/- 5ms at full depth
            float baseDelayMs = 7.0f;
            float modRangeMs = 5.0f * depth;

            float delayMsL = baseDelayMs + lfoL * modRangeMs;
            float delayMsR = baseDelayMs + lfoR * modRangeMs;

            delayMsL = std::clamp(delayMsL, 1.0f, 50.0f);
            delayMsR = std::clamp(delayMsR, 1.0f, 50.0f);

            float delaySamplesL = (delayMsL / 1000.0f) * static_cast<float>(sampleRate);
            float delaySamplesR = (delayMsR / 1000.0f) * static_cast<float>(sampleRate);

            // === BBD CLOCK RATE (for authentic coloration) ===
            // Shorter delays = higher clock rate = brighter sound
            constexpr float numStages = 512.0f; // Shorter BBD for chorus
            float clockRateL = numStages / (2.0f * (delayMsL / 1000.0f));
            float clockRateR = numStages / (2.0f * (delayMsR / 1000.0f));
            float bbdNyquistL = std::clamp(clockRateL / 2.0f, 2000.0f, 20000.0f);
            float bbdNyquistR = std::clamp(clockRateR / 2.0f, 2000.0f, 20000.0f);

            // === COMPANDING: Pre-compress ===
            float compressedL = compress(left[i]);
            float compressedR = compress(right[i]);

            // === WRITE TO DELAY LINES ===
            delayBufferL[writeIndexL] = compressedL;
            delayBufferR[writeIndexR] = compressedR;

            // === READ FROM DELAY LINES (with interpolation) ===
            float readPosL = static_cast<float>(writeIndexL) - delaySamplesL;
            float readPosR = static_cast<float>(writeIndexR) - delaySamplesR;
            if (readPosL < 0) readPosL += static_cast<float>(delayBufferL.size());
            if (readPosR < 0) readPosR += static_cast<float>(delayBufferR.size());

            int readIdx0L = static_cast<int>(readPosL);
            int readIdx1L = (readIdx0L + 1) % static_cast<int>(delayBufferL.size());
            float fracL = readPosL - static_cast<float>(readIdx0L);

            int readIdx0R = static_cast<int>(readPosR);
            int readIdx1R = (readIdx0R + 1) % static_cast<int>(delayBufferR.size());
            float fracR = readPosR - static_cast<float>(readIdx0R);

            float delayedL = delayBufferL[readIdx0L] * (1.0f - fracL) + delayBufferL[readIdx1L] * fracL;
            float delayedR = delayBufferR[readIdx0R] * (1.0f - fracR) + delayBufferR[readIdx1R] * fracR;

            // === BBD LOW-PASS FILTER ===
            delayedL = bbdLowPassL(delayedL, bbdNyquistL);
            delayedR = bbdLowPassR(delayedR, bbdNyquistR);

            // === TONE CONTROL ===
            float toneCutoff = 1000.0f + (tone * 15000.0f);
            delayedL = toneLowPassL(delayedL, toneCutoff);
            delayedR = toneLowPassR(delayedR, toneCutoff);

            // === COMPANDING: Expand ===
            float expandedL = expand(delayedL);
            float expandedR = expand(delayedR);

            // === MIX ===
            left[i] = left[i] * (1.0f - mix) + expandedL * mix;
            right[i] = right[i] * (1.0f - mix) + expandedR * mix;

            // Advance write indices
            writeIndexL = (writeIndexL + 1) % static_cast<int>(delayBufferL.size());
            writeIndexR = (writeIndexR + 1) % static_cast<int>(delayBufferR.size());
        }
    }

private:
    // === COMPANDING ===
    float compress(float x)
    {
        constexpr float mu = 8.0f;
        float sign = (x >= 0.0f) ? 1.0f : -1.0f;
        float absX = std::abs(x);
        return sign * std::log(1.0f + mu * absX) / std::log(1.0f + mu);
    }

    float expand(float x)
    {
        constexpr float mu = 8.0f;
        float sign = (x >= 0.0f) ? 1.0f : -1.0f;
        float absX = std::abs(x);
        return sign * (std::pow(1.0f + mu, absX) - 1.0f) / mu;
    }

    // === BBD LOW-PASS FILTERS (per channel) ===
    float bbdLowPassL(float input, float cutoffHz)
    {
        float omega = 2.0f * 3.14159265359f * cutoffHz / static_cast<float>(sampleRate);
        float g = omega / (1.0f + omega);
        bbdLpfStateL = bbdLpfStateL + g * (input - bbdLpfStateL);
        return bbdLpfStateL;
    }

    float bbdLowPassR(float input, float cutoffHz)
    {
        float omega = 2.0f * 3.14159265359f * cutoffHz / static_cast<float>(sampleRate);
        float g = omega / (1.0f + omega);
        bbdLpfStateR = bbdLpfStateR + g * (input - bbdLpfStateR);
        return bbdLpfStateR;
    }

    // === TONE FILTERS (per channel) ===
    float toneLowPassL(float input, float cutoffHz)
    {
        float omega = 2.0f * 3.14159265359f * cutoffHz / static_cast<float>(sampleRate);
        float g = omega / (1.0f + omega);
        toneLpfStateL = toneLpfStateL + g * (input - toneLpfStateL);
        return toneLpfStateL;
    }

    float toneLowPassR(float input, float cutoffHz)
    {
        float omega = 2.0f * 3.14159265359f * cutoffHz / static_cast<float>(sampleRate);
        float g = omega / (1.0f + omega);
        toneLpfStateR = toneLpfStateR + g * (input - toneLpfStateR);
        return toneLpfStateR;
    }

    void resetFilters()
    {
        bbdLpfStateL = 0.0f;
        bbdLpfStateR = 0.0f;
        toneLpfStateL = 0.0f;
        toneLpfStateR = 0.0f;
    }

    // Parameters
    float rate = 0.5f;      // LFO rate in Hz
    float depth = 0.5f;     // Modulation depth
    float mix = 0.5f;       // Wet/dry mix
    float tone = 0.7f;      // Tone control (brightness)

    // State
    double sampleRate = 44100.0;
    std::vector<float> delayBufferL;
    std::vector<float> delayBufferR;
    int writeIndexL = 0;
    int writeIndexR = 0;
    double lfoPhaseL = 0.0;
    double lfoPhaseR = 0.25;

    // Filter states
    float bbdLpfStateL = 0.0f;
    float bbdLpfStateR = 0.0f;
    float toneLpfStateL = 0.0f;
    float toneLpfStateR = 0.0f;
};
