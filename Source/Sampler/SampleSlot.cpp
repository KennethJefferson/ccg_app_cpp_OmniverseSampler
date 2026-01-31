#include "SampleSlot.h"

SampleSlot::SampleSlot()
{
    thumbnailData.resize(THUMBNAIL_POINTS, 0.0f);
}

bool SampleSlot::loadFromFile(const juce::File& file, juce::AudioFormatReader& reader, double targetSampleRate)
{
    if (reader.numChannels == 0 || reader.lengthInSamples == 0)
        return false;

    sourceSampleRate = reader.sampleRate;
    filePath = file.getFullPathName();
    fileName = file.getFileName();

    // Read the audio data
    juce::AudioBuffer<float> tempBuffer(static_cast<int>(reader.numChannels),
                                         static_cast<int>(reader.lengthInSamples));
    reader.read(&tempBuffer, 0, static_cast<int>(reader.lengthInSamples), 0, true, true);

    // Resample if necessary
    if (targetSampleRate > 0 && std::abs(sourceSampleRate - targetSampleRate) > 0.1)
    {
        double ratio = targetSampleRate / sourceSampleRate;
        int newNumSamples = static_cast<int>(std::ceil(tempBuffer.getNumSamples() * ratio));

        audioData.setSize(tempBuffer.getNumChannels(), newNumSamples);

        for (int channel = 0; channel < tempBuffer.getNumChannels(); ++channel)
        {
            const float* inputData = tempBuffer.getReadPointer(channel);
            float* outputData = audioData.getWritePointer(channel);

            for (int i = 0; i < newNumSamples; ++i)
            {
                double srcIndex = i / ratio;
                int srcIndexInt = static_cast<int>(srcIndex);
                float frac = static_cast<float>(srcIndex - srcIndexInt);

                if (srcIndexInt + 1 < tempBuffer.getNumSamples())
                {
                    outputData[i] = inputData[srcIndexInt] * (1.0f - frac)
                                  + inputData[srcIndexInt + 1] * frac;
                }
                else if (srcIndexInt < tempBuffer.getNumSamples())
                {
                    outputData[i] = inputData[srcIndexInt];
                }
                else
                {
                    outputData[i] = 0.0f;
                }
            }
        }

        sourceSampleRate = targetSampleRate;
    }
    else
    {
        audioData = std::move(tempBuffer);
    }

    loaded = true;
    generateThumbnail();

    return true;
}

void SampleSlot::clear()
{
    audioData.setSize(0, 0);
    sourceSampleRate = 0.0;
    loaded = false;
    filePath.clear();
    fileName.clear();
    std::fill(thumbnailData.begin(), thumbnailData.end(), 0.0f);
}

void SampleSlot::generateThumbnail()
{
    if (!loaded || audioData.getNumSamples() == 0)
    {
        std::fill(thumbnailData.begin(), thumbnailData.end(), 0.0f);
        return;
    }

    int samplesPerPoint = audioData.getNumSamples() / THUMBNAIL_POINTS;
    if (samplesPerPoint < 1) samplesPerPoint = 1;

    for (int i = 0; i < THUMBNAIL_POINTS; ++i)
    {
        int startSample = i * samplesPerPoint;
        int endSample = std::min(startSample + samplesPerPoint, audioData.getNumSamples());

        float maxVal = 0.0f;

        for (int ch = 0; ch < audioData.getNumChannels(); ++ch)
        {
            const float* data = audioData.getReadPointer(ch);
            for (int s = startSample; s < endSample; ++s)
            {
                float absVal = std::abs(data[s]);
                if (absVal > maxVal)
                    maxVal = absVal;
            }
        }

        thumbnailData[i] = maxVal;
    }
}
