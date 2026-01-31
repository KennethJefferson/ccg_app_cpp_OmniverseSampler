#pragma once

#include <cmath>
#include <algorithm>

class TapeSaturation
{
public:
    TapeSaturation() = default;

    void prepare(double newSampleRate, int /*maxBlockSize*/)
    {
        sampleRate = newSampleRate;
        resetFilters();
    }

    void reset()
    {
        resetFilters();
    }

    void setDrive(float d) { drive = std::clamp(d, 0.0f, 1.0f); }
    void setTone(float t) { tone = std::clamp(t, 0.0f, 1.0f); }
    void setMix(float m) { mix = std::clamp(m, 0.0f, 1.0f); }
    void setCompression(float c) { compression = std::clamp(c, 0.0f, 1.0f); }

    void processBlock(float* left, float* right, int numSamples)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            float dryL = left[i];
            float dryR = right[i];

            // === INPUT GAIN (drive) ===
            // Drive of 0 = 1x gain, drive of 1 = 10x gain
            float inputGain = 1.0f + drive * 9.0f;
            float wetL = dryL * inputGain;
            float wetR = dryR * inputGain;

            // === TAPE COMPRESSION ===
            // Soft knee compression before saturation
            if (compression > 0.0f)
            {
                wetL = tapeCompress(wetL);
                wetR = tapeCompress(wetR);
            }

            // === TAPE SATURATION (asymmetric for warmth) ===
            wetL = tapeSaturate(wetL);
            wetR = tapeSaturate(wetR);

            // === TAPE HEAD HIGH FREQUENCY ROLL-OFF ===
            // Simulates the frequency response of tape heads
            // Tone: 0 = dark (2kHz), 1 = bright (18kHz)
            float cutoff = 2000.0f + tone * 16000.0f;
            wetL = tapeHeadFilterL(wetL, cutoff);
            wetR = tapeHeadFilterR(wetR, cutoff);

            // === SLIGHT BASS BUMP (tape characteristic) ===
            wetL = bassBumpL(wetL);
            wetR = bassBumpR(wetR);

            // === OUTPUT LEVEL COMPENSATION ===
            // Reduce output to compensate for drive gain
            float outputComp = 1.0f / (1.0f + drive * 2.0f);
            wetL *= outputComp;
            wetR *= outputComp;

            // === MIX ===
            left[i] = dryL * (1.0f - mix) + wetL * mix;
            right[i] = dryR * (1.0f - mix) + wetR * mix;
        }
    }

private:
    // === TAPE COMPRESSION ===
    // Soft knee compression characteristic of tape
    float tapeCompress(float x)
    {
        float threshold = 0.5f;
        float ratio = 1.0f + compression * 3.0f; // 1:1 to 4:1

        float absX = std::abs(x);
        float sign = (x >= 0.0f) ? 1.0f : -1.0f;

        if (absX <= threshold)
            return x;

        // Soft knee compression above threshold
        float excess = absX - threshold;
        float compressed = threshold + excess / ratio;

        return sign * compressed;
    }

    // === TAPE SATURATION ===
    // Asymmetric saturation for analog warmth
    float tapeSaturate(float x)
    {
        // Asymmetric waveshaping - positive and negative handled differently
        // This creates even harmonics like real tape

        if (x >= 0.0f)
        {
            // Positive: softer saturation (tanh-like)
            return std::tanh(x * 1.2f);
        }
        else
        {
            // Negative: slightly harder saturation (creates asymmetry)
            float absX = std::abs(x);
            float sat = std::tanh(absX * 1.5f);
            return -sat * 0.95f; // Slight asymmetry
        }
    }

    // === TAPE HEAD FILTER (per channel) ===
    // 2-pole Butterworth low-pass
    float tapeHeadFilterL(float input, float cutoffHz)
    {
        float omega = 2.0f * 3.14159265359f * cutoffHz / static_cast<float>(sampleRate);
        float g = omega / (1.0f + omega);

        // Two cascaded one-pole filters for 2-pole response
        tapeHpState1L = tapeHpState1L + g * (input - tapeHpState1L);
        tapeHpState2L = tapeHpState2L + g * (tapeHpState1L - tapeHpState2L);

        return tapeHpState2L;
    }

    float tapeHeadFilterR(float input, float cutoffHz)
    {
        float omega = 2.0f * 3.14159265359f * cutoffHz / static_cast<float>(sampleRate);
        float g = omega / (1.0f + omega);

        tapeHpState1R = tapeHpState1R + g * (input - tapeHpState1R);
        tapeHpState2R = tapeHpState2R + g * (tapeHpState1R - tapeHpState2R);

        return tapeHpState2R;
    }

    // === BASS BUMP (subtle low frequency emphasis) ===
    // Simulates tape machine's low-end warmth
    float bassBumpL(float input)
    {
        // Very subtle high-pass at 30Hz to remove DC
        float hpOmega = 2.0f * 3.14159265359f * 30.0f / static_cast<float>(sampleRate);
        float hpG = hpOmega / (1.0f + hpOmega);
        dcBlockStateL = dcBlockStateL + hpG * (input - dcBlockStateL);
        float hp = input - dcBlockStateL;

        // Low shelf boost around 100Hz
        float lsOmega = 2.0f * 3.14159265359f * 100.0f / static_cast<float>(sampleRate);
        float lsG = lsOmega / (1.0f + lsOmega);
        bassStateL = bassStateL + lsG * (hp - bassStateL);

        // Add subtle bass boost (15%)
        return hp + bassStateL * 0.15f * drive;
    }

    float bassBumpR(float input)
    {
        float hpOmega = 2.0f * 3.14159265359f * 30.0f / static_cast<float>(sampleRate);
        float hpG = hpOmega / (1.0f + hpOmega);
        dcBlockStateR = dcBlockStateR + hpG * (input - dcBlockStateR);
        float hp = input - dcBlockStateR;

        float lsOmega = 2.0f * 3.14159265359f * 100.0f / static_cast<float>(sampleRate);
        float lsG = lsOmega / (1.0f + lsOmega);
        bassStateR = bassStateR + lsG * (hp - bassStateR);

        return hp + bassStateR * 0.15f * drive;
    }

    void resetFilters()
    {
        tapeHpState1L = 0.0f;
        tapeHpState2L = 0.0f;
        tapeHpState1R = 0.0f;
        tapeHpState2R = 0.0f;
        dcBlockStateL = 0.0f;
        dcBlockStateR = 0.0f;
        bassStateL = 0.0f;
        bassStateR = 0.0f;
    }

    // Parameters
    float drive = 0.3f;         // Input drive
    float tone = 0.7f;          // High frequency rolloff
    float mix = 0.5f;           // Wet/dry mix
    float compression = 0.3f;   // Tape compression amount

    // State
    double sampleRate = 44100.0;

    // Filter states
    float tapeHpState1L = 0.0f;
    float tapeHpState2L = 0.0f;
    float tapeHpState1R = 0.0f;
    float tapeHpState2R = 0.0f;
    float dcBlockStateL = 0.0f;
    float dcBlockStateR = 0.0f;
    float bassStateL = 0.0f;
    float bassStateR = 0.0f;
};
