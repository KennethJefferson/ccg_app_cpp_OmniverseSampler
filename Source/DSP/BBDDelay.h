#pragma once

#include <vector>
#include <cmath>
#include <algorithm>

class BBDDelay
{
public:
    BBDDelay() = default;

    void prepare(double newSampleRate, int maxBlockSize)
    {
        sampleRate = newSampleRate;

        // Max delay of 1 second
        int maxDelaySamples = static_cast<int>(sampleRate * 1.0);
        delayBuffer.resize(maxDelaySamples, 0.0f);

        // Initialize modulation LFO
        modPhase = 0.0;

        // Initialize filters
        resetFilters();
    }

    void reset()
    {
        std::fill(delayBuffer.begin(), delayBuffer.end(), 0.0f);
        writeIndex = 0;
        modPhase = 0.0;
        resetFilters();
    }

    void setDelayTime(float timeMs) { delayTimeMs = std::clamp(timeMs, 1.0f, 1000.0f); }
    void setFeedback(float fb) { feedback = std::clamp(fb, 0.0f, 0.99f); }
    void setModDepth(float depth) { modDepth = std::clamp(depth, 0.0f, 1.0f); }
    void setModRate(float rate) { modRate = std::clamp(rate, 0.01f, 10.0f); }
    void setTone(float t) { tone = std::clamp(t, 0.0f, 1.0f); }
    void setMix(float m) { mix = std::clamp(m, 0.0f, 1.0f); }

    float process(float input)
    {
        if (delayBuffer.empty())
            return input;

        // Sanitize input
        if (!std::isfinite(input))
            input = 0.0f;

        // === COMPANDING: Pre-compress input ===
        float compressedInput = compress(input);

        // === MODULATION: Calculate modulated delay time ===
        float lfoValue = std::sin(2.0 * 3.14159265359 * modPhase);
        modPhase += modRate / sampleRate;
        if (modPhase >= 1.0)
            modPhase -= 1.0;

        // Modulation range: +/- 2ms at full depth
        float modulationMs = lfoValue * modDepth * 2.0f;
        float actualDelayMs = delayTimeMs + modulationMs;
        actualDelayMs = std::clamp(actualDelayMs, 1.0f, 1000.0f);

        float delaySamples = (actualDelayMs / 1000.0f) * static_cast<float>(sampleRate);

        // === BBD CLOCK RATE CALCULATION ===
        // Real BBD: clockRate = numStages / (2 * delayTime)
        // This determines the Nyquist frequency (bandwidth)
        // Simulating a 1024-stage BBD (like MN3207)
        constexpr float numStages = 1024.0f;
        float delayTimeSeconds = actualDelayMs / 1000.0f;
        float clockRate = numStages / (2.0f * delayTimeSeconds);
        float bbdNyquist = clockRate / 2.0f;

        // Clamp nyquist to reasonable range
        bbdNyquist = std::clamp(bbdNyquist, 200.0f, 20000.0f);

        // === READ FROM DELAY LINE (with interpolation) ===
        float readPos = static_cast<float>(writeIndex) - delaySamples;
        if (readPos < 0)
            readPos += static_cast<float>(delayBuffer.size());

        int readIndex0 = static_cast<int>(readPos);
        int readIndex1 = (readIndex0 + 1) % static_cast<int>(delayBuffer.size());
        float frac = readPos - static_cast<float>(readIndex0);

        // Linear interpolation (could use allpass for better quality)
        float delayedSample = delayBuffer[readIndex0] * (1.0f - frac)
                            + delayBuffer[readIndex1] * frac;

        // === BBD LOW-PASS FILTER (clock-rate dependent) ===
        // This is what gives BBD its characteristic "dark" sound at long delays
        delayedSample = bbdLowPass(delayedSample, bbdNyquist);

        // === TONE CONTROL (user-adjustable additional filtering) ===
        // tone 0 = dark, tone 1 = bright
        float toneCutoff = 200.0f + (tone * 10000.0f);
        delayedSample = toneLowPass(delayedSample, toneCutoff);

        // === COMPANDING: Expand output ===
        float expandedOutput = expand(delayedSample);

        // === FEEDBACK PATH with soft saturation ===
        float feedbackSample = softSaturate(expandedOutput * feedback);

        // Compress feedback before writing back
        float toWrite = compressedInput + compress(feedbackSample);

        // Sanitize and clamp to prevent runaway
        if (!std::isfinite(toWrite))
            toWrite = 0.0f;
        toWrite = std::clamp(toWrite, -4.0f, 4.0f);

        // === WRITE TO DELAY LINE ===
        delayBuffer[writeIndex] = toWrite;
        writeIndex = (writeIndex + 1) % static_cast<int>(delayBuffer.size());

        // === MIX ===
        float output = input * (1.0f - mix) + expandedOutput * mix;

        // Final sanitization
        if (!std::isfinite(output))
            output = input;

        return output;
    }

    void processBlock(float* left, float* right, int numSamples)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            // Process mono sum for delay, then apply to both channels
            float mono = (left[i] + right[i]) * 0.5f;
            float delayed = process(mono);

            // Mix delayed signal back to stereo
            left[i] = left[i] * (1.0f - mix) + delayed * mix;
            right[i] = right[i] * (1.0f - mix) + delayed * mix;
        }
    }

private:
    // === COMPANDING (simulates BBD compander chips) ===
    float compress(float x)
    {
        // Simple mu-law style compression
        constexpr float mu = 10.0f;
        float sign = (x >= 0.0f) ? 1.0f : -1.0f;
        float absX = std::abs(x);
        return sign * std::log(1.0f + mu * absX) / std::log(1.0f + mu);
    }

    float expand(float x)
    {
        // Inverse of compress
        constexpr float mu = 10.0f;
        float sign = (x >= 0.0f) ? 1.0f : -1.0f;
        float absX = std::abs(x);
        return sign * (std::pow(1.0f + mu, absX) - 1.0f) / mu;
    }

    // === SOFT SATURATION (tape/tube-like) ===
    float softSaturate(float x)
    {
        // Soft clipping using tanh
        return std::tanh(x * 1.5f) / std::tanh(1.5f);
    }

    // === BBD LOW-PASS FILTER ===
    // Simple one-pole LPF that models the bandwidth limitation
    float bbdLowPass(float input, float cutoffHz)
    {
        float omega = 2.0f * 3.14159265359f * cutoffHz / static_cast<float>(sampleRate);
        float g = omega / (1.0f + omega);

        bbdLpfState = bbdLpfState + g * (input - bbdLpfState);

        // Protect filter state from corruption
        if (!std::isfinite(bbdLpfState))
            bbdLpfState = 0.0f;

        return bbdLpfState;
    }

    // === TONE LOW-PASS FILTER ===
    float toneLowPass(float input, float cutoffHz)
    {
        float omega = 2.0f * 3.14159265359f * cutoffHz / static_cast<float>(sampleRate);
        float g = omega / (1.0f + omega);

        toneLpfState = toneLpfState + g * (input - toneLpfState);

        // Protect filter state from corruption
        if (!std::isfinite(toneLpfState))
            toneLpfState = 0.0f;

        return toneLpfState;
    }

    void resetFilters()
    {
        bbdLpfState = 0.0f;
        toneLpfState = 0.0f;
    }

    // Parameters
    float delayTimeMs = 200.0f;
    float feedback = 0.5f;
    float modDepth = 0.0f;
    float modRate = 1.0f;
    float tone = 0.5f;
    float mix = 0.5f;

    // State
    double sampleRate = 44100.0;
    std::vector<float> delayBuffer;
    int writeIndex = 0;
    double modPhase = 0.0;

    // Filter states
    float bbdLpfState = 0.0f;
    float toneLpfState = 0.0f;
};
