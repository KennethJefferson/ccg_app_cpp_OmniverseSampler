#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_formats/juce_audio_formats.h>

class SampleSlot
{
public:
    SampleSlot();
    ~SampleSlot() = default;

    bool loadFromFile(const juce::File& file, juce::AudioFormatReader& reader, double targetSampleRate);
    void clear();

    bool isLoaded() const { return loaded; }
    const juce::AudioBuffer<float>& getAudioData() const { return audioData; }
    int getNumSamples() const { return audioData.getNumSamples(); }
    int getNumChannels() const { return audioData.getNumChannels(); }
    double getSourceSampleRate() const { return sourceSampleRate; }
    juce::String getFilePath() const { return filePath; }
    juce::String getFileName() const { return fileName; }

    // For waveform display
    const std::vector<float>& getThumbnailData() const { return thumbnailData; }

private:
    void generateThumbnail();

    juce::AudioBuffer<float> audioData;
    double sourceSampleRate = 0.0;
    bool loaded = false;
    juce::String filePath;
    juce::String fileName;

    std::vector<float> thumbnailData;
    static constexpr int THUMBNAIL_POINTS = 256;
};
