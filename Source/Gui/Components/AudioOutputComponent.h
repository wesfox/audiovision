#pragma once

#include <JuceHeader.h>

#include "AudioEngine/AudioOutputEngine.h"

class AudioOutputComponent : public juce::Component
{
public:
    explicit AudioOutputComponent(AudioOutputEngine& engine);
    ~AudioOutputComponent() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    AudioOutputEngine& audioEngine;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioOutputComponent)
};
