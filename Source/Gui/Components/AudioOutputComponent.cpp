#include "AudioOutputComponent.h"

AudioOutputComponent::AudioOutputComponent(AudioOutputEngine& engine)
    : audioEngine(engine)
{
}

void AudioOutputComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::transparentBlack);
}

void AudioOutputComponent::resized()
{
}
