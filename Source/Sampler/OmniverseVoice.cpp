#include "OmniverseVoice.h"
#include "../Utils/Parameters.h"

OmniverseVoice::OmniverseVoice()
{
    for (int i = 0; i < 5; ++i)
        activeSlotIndices.push_back(i);
}

void OmniverseVoice::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    currentSampleRate = sampleRate;

    // Prepare filters and LFOs
    for (int i = 0; i < 5; ++i)
    {
        filtersL[i].prepare(sampleRate);
        filtersR[i].prepare(sampleRate);
        lfos[i].prepare(sampleRate);
    }
}

bool OmniverseVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return sound != nullptr;
}

void OmniverseVoice::startNote(int midiNoteNumber, float velocity,
                                juce::SynthesiserSound*, int /*currentPitchWheelPosition*/)
{
    midiNote = midiNoteNumber + (octaveShift * 12);
    noteVelocity = velocity;

    // Reset all slot states for new note
    for (int i = 0; i < 5; ++i)
    {
        auto& state = slotStates[i];
        state.samplePosition = 0.0;
        state.envelopeTime = 0.0;
        state.envelopeValue = 0.0f;
        state.inRelease = false;
        state.releaseStartValue = 0.0f;
        state.releaseTime = 0.0;
        state.isPlaying = true;

        // Reset filters and LFOs
        filtersL[i].reset();
        filtersR[i].reset();
        lfos[i].reset();
    }

    controlRateCounter = 0;
}

void OmniverseVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        for (int i = 0; i < 5; ++i)
        {
            slotStates[i].inRelease = true;
            slotStates[i].releaseStartValue = slotStates[i].envelopeValue;
            slotStates[i].releaseTime = 0.0;
        }
    }
    else
    {
        for (auto& state : slotStates)
        {
            state.isPlaying = false;
        }
        clearCurrentNote();
    }
}

void OmniverseVoice::pitchWheelMoved(int /*newPitchWheelValue*/)
{
}

void OmniverseVoice::controllerMoved(int /*controllerNumber*/, int /*newControllerValue*/)
{
}

float OmniverseVoice::getParameter(const juce::String& paramId)
{
    if (apvts == nullptr)
        return 0.0f;

    auto* param = apvts->getRawParameterValue(paramId);
    return param != nullptr ? param->load() : 0.0f;
}

void OmniverseVoice::updateFilterParameters(int slotIndex)
{
    if (apvts == nullptr)
        return;

    // Get filter parameters
    int filterType = static_cast<int>(getParameter(Parameters::slotFilterType(slotIndex)));
    float baseCutoff = getParameter(Parameters::slotFilterCutoff(slotIndex));
    float resonance = getParameter(Parameters::slotFilterResonance(slotIndex));
    bool bypass = getParameter(Parameters::slotFilterBypass(slotIndex)) > 0.5f;

    // Get LFO parameters
    float lfoRate = getParameter(Parameters::slotLfoRate(slotIndex));
    float lfoDepth = getParameter(Parameters::slotLfoDepth(slotIndex));
    int lfoWaveform = static_cast<int>(getParameter(Parameters::slotLfoWaveform(slotIndex)));

    // Update LFO
    lfos[slotIndex].setRate(lfoRate);
    lfos[slotIndex].setWaveform(static_cast<LFO::Waveform>(lfoWaveform));

    // Calculate modulated cutoff
    float lfoValue = lfos[slotIndex].getCurrentValue(); // -1 to 1
    float modulationRange = baseCutoff * lfoDepth; // Modulate by percentage of base cutoff
    float modulatedCutoff = baseCutoff + (lfoValue * modulationRange);

    // Clamp cutoff to valid range
    modulatedCutoff = std::clamp(modulatedCutoff, 20.0f, 20000.0f);

    // Set filter type
    SVFilter::Type type;
    switch (filterType)
    {
        case 0: type = SVFilter::Type::LowPass; break;
        case 1: type = SVFilter::Type::HighPass; break;
        case 2: type = SVFilter::Type::BandPass; break;
        default: type = SVFilter::Type::LowPass; break;
    }

    // Apply to both channels
    if (!bypass)
    {
        filtersL[slotIndex].setType(type);
        filtersL[slotIndex].setCutoff(modulatedCutoff);
        filtersL[slotIndex].setResonance(resonance);

        filtersR[slotIndex].setType(type);
        filtersR[slotIndex].setCutoff(modulatedCutoff);
        filtersR[slotIndex].setResonance(resonance);
    }
}

float OmniverseVoice::calculateEnvelope(int slotIndex, SlotState& state, bool noteIsHeld)
{
    float attackMs = getParameter(Parameters::slotAttack(slotIndex));
    float decayMs = getParameter(Parameters::slotDecay(slotIndex));
    float sustainDb = getParameter(Parameters::slotSustain(slotIndex));
    float releaseMs = getParameter(Parameters::slotRelease(slotIndex));

    float attackSamples = (attackMs / 1000.0f) * static_cast<float>(currentSampleRate);
    float decaySamples = (decayMs / 1000.0f) * static_cast<float>(currentSampleRate);
    float releaseSamples = (releaseMs / 1000.0f) * static_cast<float>(currentSampleRate);
    float sustainLevel = juce::Decibels::decibelsToGain(sustainDb);

    if (attackSamples < 1.0f) attackSamples = 1.0f;
    if (decaySamples < 1.0f) decaySamples = 1.0f;
    if (releaseSamples < 1.0f) releaseSamples = 1.0f;

    if (state.inRelease)
    {
        float releaseProgress = static_cast<float>(state.releaseTime) / releaseSamples;
        if (releaseProgress >= 1.0f)
        {
            state.isPlaying = false;
            return 0.0f;
        }
        return state.releaseStartValue * (1.0f - releaseProgress);
    }

    double envTime = state.envelopeTime;

    if (envTime < attackSamples)
    {
        return static_cast<float>(envTime / attackSamples);
    }

    double decayPosition = envTime - attackSamples;
    if (decayPosition < decaySamples)
    {
        float decayProgress = static_cast<float>(decayPosition / decaySamples);
        return 1.0f - (decayProgress * (1.0f - sustainLevel));
    }

    if (noteIsHeld)
    {
        return sustainLevel;
    }

    return sustainLevel;
}

void OmniverseVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                      int startSample, int numSamples)
{
    if (sampleSlots == nullptr || apvts == nullptr)
        return;

    if (!isVoiceActive())
        return;

    bool anySlotStillPlaying = false;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Update filter/LFO parameters at control rate
        if (++controlRateCounter >= CONTROL_RATE_DIVIDER)
        {
            controlRateCounter = 0;
            for (int slotIdx : activeSlotIndices)
            {
                updateFilterParameters(slotIdx);
            }
        }

        // Advance LFOs every sample
        for (int slotIdx : activeSlotIndices)
        {
            lfos[slotIdx].process();
        }

        float leftSample = 0.0f;
        float rightSample = 0.0f;

        for (int slotIdx : activeSlotIndices)
        {
            auto* slot = (*sampleSlots)[slotIdx];
            if (slot == nullptr || !slot->isLoaded())
                continue;

            auto& state = slotStates[slotIdx];

            if (!state.isPlaying)
                continue;

            const auto& audioData = slot->getAudioData();
            int numSlotSamples = audioData.getNumSamples();
            int numChannels = audioData.getNumChannels();

            if (numSlotSamples == 0)
                continue;

            // Get in/out points (0-100%)
            float inPointPercent = getParameter(Parameters::slotInPoint(slotIdx));
            float outPointPercent = getParameter(Parameters::slotOutPoint(slotIdx));

            // Ensure in < out with minimum 1% gap
            if (outPointPercent <= inPointPercent + 1.0f)
                outPointPercent = inPointPercent + 1.0f;

            // Calculate sample positions for in/out points
            int inSample = static_cast<int>((inPointPercent / 100.0f) * numSlotSamples);
            int outSample = static_cast<int>((outPointPercent / 100.0f) * numSlotSamples);

            // Clamp to valid range
            inSample = std::clamp(inSample, 0, numSlotSamples - 1);
            outSample = std::clamp(outSample, inSample + 1, numSlotSamples);

            // Enforce minimum playable length (at least 64 samples to prevent clicks)
            int playableLength = outSample - inSample;
            if (playableLength < 64)
            {
                outSample = std::min(inSample + 64, numSlotSamples);
                playableLength = outSample - inSample;
            }

            float pitchSemitones = getParameter(Parameters::slotPitch(slotIdx));
            float totalPitchShift = (midiNote - ROOT_NOTE) + pitchSemitones;
            double pitchRatio = std::pow(2.0, totalPitchShift / 12.0);

            bool noteIsHeld = !state.inRelease;
            float envelope = calculateEnvelope(slotIdx, state, noteIsHeld);
            state.envelopeValue = envelope;

            float volumeDb = getParameter(Parameters::slotVolume(slotIdx));
            float volume = juce::Decibels::decibelsToGain(volumeDb) * noteVelocity;

            // Calculate read position within the in/out range
            double readPosition;
            if (isReversed)
            {
                // Reverse: start from out point, go towards in point
                readPosition = (outSample - 1) - state.samplePosition;
            }
            else
            {
                // Normal: start from in point, go towards out point
                readPosition = inSample + state.samplePosition;
            }

            // Check if we're within the playable range
            bool withinRange = isReversed
                ? (readPosition >= inSample && readPosition < outSample)
                : (readPosition >= inSample && readPosition < outSample);

            if (withinRange && state.isPlaying)
            {
                anySlotStillPlaying = true;

                int pos0 = static_cast<int>(readPosition);
                int pos1 = pos0 + 1;
                float frac = static_cast<float>(readPosition - pos0);

                if (pos1 >= numSlotSamples)
                    pos1 = numSlotSamples - 1;

                float leftVal = 0.0f;
                float rightVal = 0.0f;

                if (numChannels >= 1)
                {
                    const float* channelData = audioData.getReadPointer(0);
                    leftVal = channelData[pos0] * (1.0f - frac) + channelData[pos1] * frac;
                }

                if (numChannels >= 2)
                {
                    const float* channelData = audioData.getReadPointer(1);
                    rightVal = channelData[pos0] * (1.0f - frac) + channelData[pos1] * frac;
                }
                else
                {
                    rightVal = leftVal;
                }

                // Apply filter (if not bypassed)
                bool filterBypass = getParameter(Parameters::slotFilterBypass(slotIdx)) > 0.5f;
                if (!filterBypass)
                {
                    leftVal = filtersL[slotIdx].process(leftVal);
                    rightVal = filtersR[slotIdx].process(rightVal);
                }

                float gain = envelope * volume;
                leftSample += leftVal * gain;
                rightSample += rightVal * gain;

                // Sanitize to prevent NaN/Inf propagation
                if (!std::isfinite(leftSample)) leftSample = 0.0f;
                if (!std::isfinite(rightSample)) rightSample = 0.0f;
            }
            else if (state.samplePosition >= playableLength)
            {
                // Reached end of playable region
                if (!state.inRelease)
                {
                    state.inRelease = true;
                    state.releaseStartValue = state.envelopeValue;
                    state.releaseTime = 0.0;
                }
            }

            state.samplePosition += pitchRatio;
            state.envelopeTime += 1.0;

            if (state.inRelease)
            {
                state.releaseTime += 1.0;
            }

            // Check for loop mode
            bool loopEnabled = getParameter(Parameters::slotLoop(slotIdx)) > 0.5f;
            if (loopEnabled && !state.inRelease && state.samplePosition >= playableLength)
            {
                // Loop back to start of playable region
                state.samplePosition = std::fmod(state.samplePosition, static_cast<double>(playableLength));
            }

            if (state.isPlaying && envelope > 0.0001f)
            {
                anySlotStillPlaying = true;
            }
        }

        if (outputBuffer.getNumChannels() >= 1)
            outputBuffer.addSample(0, startSample + sample, leftSample);
        if (outputBuffer.getNumChannels() >= 2)
            outputBuffer.addSample(1, startSample + sample, rightSample);
    }

    if (!anySlotStillPlaying)
    {
        clearCurrentNote();
    }
}
