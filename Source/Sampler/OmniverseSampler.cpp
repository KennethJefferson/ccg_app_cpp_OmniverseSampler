#include "OmniverseSampler.h"
#include "../Utils/Parameters.h"

OmniverseSampler::OmniverseSampler()
{
    for (int i = 0; i < NUM_SLOTS; ++i)
    {
        slotPointers[i] = &slots[i];
    }

    addSound(new OmniverseSound());

    for (int i = 0; i < NUM_VOICES; ++i)
    {
        auto* voice = new OmniverseVoice();
        voice->setSlots(&slotPointers);
        addVoice(voice);
    }
}

void OmniverseSampler::setAPVTS(juce::AudioProcessorValueTreeState* apvtsPtr)
{
    apvts = apvtsPtr;

    for (int i = 0; i < getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<OmniverseVoice*>(getVoice(i)))
        {
            voice->setAPVTS(apvts);
        }
    }
}

SampleSlot* OmniverseSampler::getSlot(int index)
{
    if (index >= 0 && index < NUM_SLOTS)
        return &slots[index];
    return nullptr;
}

const SampleSlot* OmniverseSampler::getSlot(int index) const
{
    if (index >= 0 && index < NUM_SLOTS)
        return &slots[index];
    return nullptr;
}

std::vector<int> OmniverseSampler::determineActiveSlots()
{
    if (apvts == nullptr)
    {
        std::vector<int> all;
        for (int i = 0; i < NUM_SLOTS; ++i)
            all.push_back(i);
        return all;
    }

    bool layerMode = apvts->getRawParameterValue(Parameters::PLAYBACK_LAYER)->load() > 0.5f;
    bool randomMode = apvts->getRawParameterValue(Parameters::PLAYBACK_RANDOM)->load() > 0.5f;

    // Find which slots have samples loaded
    std::vector<int> loadedSlots;
    for (int i = 0; i < NUM_SLOTS; ++i)
    {
        if (slots[i].isLoaded())
            loadedSlots.push_back(i);
    }

    if (loadedSlots.empty())
        return loadedSlots;

    if (layerMode && !randomMode)
    {
        // Layer mode: play all loaded slots
        return loadedSlots;
    }
    else if (randomMode)
    {
        // Random mode: pick one random slot
        int randomIdx = random.nextInt(static_cast<int>(loadedSlots.size()));
        return { loadedSlots[randomIdx] };
    }
    else
    {
        // Round robin mode
        roundRobinIndex = (roundRobinIndex + 1) % static_cast<int>(loadedSlots.size());
        return { loadedSlots[roundRobinIndex] };
    }
}

int OmniverseSampler::getOctaveShift()
{
    if (apvts == nullptr)
        return 0;

    bool randomOctave = apvts->getRawParameterValue(Parameters::RANDOM_OCTAVE)->load() > 0.5f;

    if (randomOctave)
    {
        // 60% chance of no shift, 20% chance of -1 octave, 20% chance of +1 octave
        float r = random.nextFloat();
        if (r < 0.2f)
            return -1;
        else if (r < 0.4f)
            return 1;
    }

    return 0;
}

void OmniverseSampler::noteOn(int midiChannel, int midiNoteNumber, float velocity)
{
    auto activeSlots = determineActiveSlots();

    if (activeSlots.empty())
        return;

    int octaveShift = getOctaveShift();
    bool reverse = apvts != nullptr &&
                   apvts->getRawParameterValue(Parameters::REVERSE)->load() > 0.5f;

    // Find a free voice
    for (auto* sound : sounds)
    {
        if (sound->appliesToNote(midiNoteNumber) && sound->appliesToChannel(midiChannel))
        {
            auto* voice = findFreeVoice(sound, midiChannel, midiNoteNumber, true);

            if (auto* omniverseVoice = dynamic_cast<OmniverseVoice*>(voice))
            {
                omniverseVoice->setActiveSlots(activeSlots);
                omniverseVoice->setReverse(reverse);
                omniverseVoice->setOctaveShift(octaveShift);

                startVoice(voice, sound, midiChannel, midiNoteNumber, velocity);
            }

            break;
        }
    }
}
