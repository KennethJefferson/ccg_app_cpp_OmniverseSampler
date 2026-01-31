#include "PluginProcessor.h"
#include "PluginEditor.h"

OmniverseAudioProcessor::OmniverseAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts(*this, nullptr, "Parameters", Parameters::createParameterLayout())
{
    formatManager.registerBasicFormats();
    sampler.setAPVTS(&apvts);
}

OmniverseAudioProcessor::~OmniverseAudioProcessor()
{
}

const juce::String OmniverseAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OmniverseAudioProcessor::acceptsMidi() const
{
    return true;
}

bool OmniverseAudioProcessor::producesMidi() const
{
    return false;
}

bool OmniverseAudioProcessor::isMidiEffect() const
{
    return false;
}

double OmniverseAudioProcessor::getTailLengthSeconds() const
{
    // Return max delay time for tail
    return 1.0;
}

int OmniverseAudioProcessor::getNumPrograms()
{
    return 1;
}

int OmniverseAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OmniverseAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String OmniverseAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void OmniverseAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void OmniverseAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    sampler.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < sampler.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<OmniverseVoice*>(sampler.getVoice(i)))
        {
            voice->prepareToPlay(sampleRate, samplesPerBlock);
        }
    }

    // Prepare BBD Delay
    bbdDelay.prepare(sampleRate, samplesPerBlock);

    // Prepare BBD Chorus
    bbdChorus.prepare(sampleRate, samplesPerBlock);

    // Prepare Tape Saturation
    tapeSaturation.prepare(sampleRate, samplesPerBlock);

    // Prepare Spectral Filter
    spectralFilter.prepare(sampleRate, samplesPerBlock);
}

void OmniverseAudioProcessor::releaseResources()
{
}

bool OmniverseAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void OmniverseAudioProcessor::updateDelayParameters()
{
    bbdDelay.setDelayTime(apvts.getRawParameterValue(Parameters::DELAY_TIME)->load());
    bbdDelay.setFeedback(apvts.getRawParameterValue(Parameters::DELAY_FEEDBACK)->load());
    bbdDelay.setModDepth(apvts.getRawParameterValue(Parameters::DELAY_MOD_DEPTH)->load());
    bbdDelay.setModRate(apvts.getRawParameterValue(Parameters::DELAY_MOD_RATE)->load());
    bbdDelay.setTone(apvts.getRawParameterValue(Parameters::DELAY_TONE)->load());
    bbdDelay.setMix(apvts.getRawParameterValue(Parameters::DELAY_MIX)->load());
}

void OmniverseAudioProcessor::updateChorusParameters()
{
    bbdChorus.setRate(apvts.getRawParameterValue(Parameters::CHORUS_RATE)->load());
    bbdChorus.setDepth(apvts.getRawParameterValue(Parameters::CHORUS_DEPTH)->load());
    bbdChorus.setTone(apvts.getRawParameterValue(Parameters::CHORUS_TONE)->load());
    bbdChorus.setMix(apvts.getRawParameterValue(Parameters::CHORUS_MIX)->load());
}

void OmniverseAudioProcessor::updateTapeParameters()
{
    tapeSaturation.setDrive(apvts.getRawParameterValue(Parameters::TAPE_DRIVE)->load());
    tapeSaturation.setCompression(apvts.getRawParameterValue(Parameters::TAPE_COMPRESSION)->load());
    tapeSaturation.setTone(apvts.getRawParameterValue(Parameters::TAPE_TONE)->load());
    tapeSaturation.setMix(apvts.getRawParameterValue(Parameters::TAPE_MIX)->load());
}

void OmniverseAudioProcessor::updateSpectralParameters()
{
    spectralFilter.setLowGain(apvts.getRawParameterValue(Parameters::SPECTRAL_LOW)->load());
    spectralFilter.setMidGain(apvts.getRawParameterValue(Parameters::SPECTRAL_MID)->load());
    spectralFilter.setHighGain(apvts.getRawParameterValue(Parameters::SPECTRAL_HIGH)->load());
    spectralFilter.setSpread(apvts.getRawParameterValue(Parameters::SPECTRAL_SPREAD)->load());
    spectralFilter.setMix(apvts.getRawParameterValue(Parameters::SPECTRAL_MIX)->load());
}

void OmniverseAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    buffer.clear();
    sampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    // Update and apply BBD Delay (if not bypassed)
    bool delayBypassed = apvts.getRawParameterValue(Parameters::DELAY_BYPASS)->load() > 0.5f;

    if (!delayBypassed && buffer.getNumChannels() >= 2)
    {
        updateDelayParameters();

        float* leftChannel = buffer.getWritePointer(0);
        float* rightChannel = buffer.getWritePointer(1);

        bbdDelay.processBlock(leftChannel, rightChannel, buffer.getNumSamples());
    }

    // Update and apply BBD Chorus (if not bypassed)
    bool chorusBypassed = apvts.getRawParameterValue(Parameters::CHORUS_BYPASS)->load() > 0.5f;

    if (!chorusBypassed && buffer.getNumChannels() >= 2)
    {
        updateChorusParameters();

        float* leftChannel = buffer.getWritePointer(0);
        float* rightChannel = buffer.getWritePointer(1);

        bbdChorus.processBlock(leftChannel, rightChannel, buffer.getNumSamples());
    }

    // Update and apply Tape Saturation (if not bypassed)
    bool tapeBypassed = apvts.getRawParameterValue(Parameters::TAPE_BYPASS)->load() > 0.5f;

    if (!tapeBypassed && buffer.getNumChannels() >= 2)
    {
        updateTapeParameters();

        float* leftChannel = buffer.getWritePointer(0);
        float* rightChannel = buffer.getWritePointer(1);

        tapeSaturation.processBlock(leftChannel, rightChannel, buffer.getNumSamples());
    }

    // Update and apply Spectral Filter (if not bypassed)
    bool spectralBypassed = apvts.getRawParameterValue(Parameters::SPECTRAL_BYPASS)->load() > 0.5f;

    if (!spectralBypassed && buffer.getNumChannels() >= 2)
    {
        updateSpectralParameters();

        float* leftChannel = buffer.getWritePointer(0);
        float* rightChannel = buffer.getWritePointer(1);

        spectralFilter.processBlock(leftChannel, rightChannel, buffer.getNumSamples());
    }

    // Apply master volume
    float masterVolume = juce::Decibels::decibelsToGain(
        apvts.getRawParameterValue(Parameters::MASTER_VOLUME)->load()
    );
    buffer.applyGain(masterVolume);

    // Apply stereo width (mid-side processing)
    float stereoWidth = apvts.getRawParameterValue(Parameters::STEREO_WIDTH)->load() / 100.0f;

    if (buffer.getNumChannels() == 2 && stereoWidth != 1.0f)
    {
        auto* leftChannel = buffer.getWritePointer(0);
        auto* rightChannel = buffer.getWritePointer(1);

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float mid = (leftChannel[i] + rightChannel[i]) * 0.5f;
            float side = (leftChannel[i] - rightChannel[i]) * 0.5f;

            side *= stereoWidth;

            leftChannel[i] = mid + side;
            rightChannel[i] = mid - side;
        }
    }

    // Final safety pass: sanitize and soft-clip output
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            // Replace NaN/Inf with silence
            if (!std::isfinite(channelData[i]))
                channelData[i] = 0.0f;

            // Soft clip to prevent harsh digital clipping
            if (std::abs(channelData[i]) > 1.0f)
                channelData[i] = std::tanh(channelData[i]);
        }
    }
}

bool OmniverseAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* OmniverseAudioProcessor::createEditor()
{
    return new OmniverseAudioProcessorEditor(*this);
}

void OmniverseAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();

    // Store sample file paths
    for (int i = 0; i < OmniverseSampler::NUM_SLOTS; ++i)
    {
        auto* slot = sampler.getSlot(i);
        if (slot && slot->isLoaded())
        {
            state.setProperty(juce::Identifier("slot_" + juce::String(i) + "_file"),
                             slot->getFilePath(), nullptr);
        }
    }

    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void OmniverseAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
    {
        auto state = juce::ValueTree::fromXml(*xmlState);
        apvts.replaceState(state);

        // Reload samples from stored paths
        for (int i = 0; i < OmniverseSampler::NUM_SLOTS; ++i)
        {
            auto filePath = state.getProperty(juce::Identifier("slot_" + juce::String(i) + "_file"), "").toString();
            if (filePath.isNotEmpty())
            {
                juce::File file(filePath);
                if (file.existsAsFile())
                {
                    loadSampleIntoSlot(i, file);
                }
            }
        }
    }
}

bool OmniverseAudioProcessor::loadSampleIntoSlot(int slotIndex, const juce::File& file)
{
    if (slotIndex < 0 || slotIndex >= OmniverseSampler::NUM_SLOTS)
        return false;

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

    if (reader == nullptr)
        return false;

    auto* slot = sampler.getSlot(slotIndex);
    if (slot == nullptr)
        return false;

    return slot->loadFromFile(file, *reader, getSampleRate());
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OmniverseAudioProcessor();
}
