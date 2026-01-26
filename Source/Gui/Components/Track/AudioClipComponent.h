#pragma once

#include <JuceHeader.h>

class AudioClip;

class AudioClipComponent : public juce::Component {
public:
    explicit AudioClipComponent(const AudioClip& clip, juce::Colour colour);

    const AudioClip& getClip() const;

    void paint(juce::Graphics& g) override;

private:
    const AudioClip& clip;
    const juce::Colour colour;
};
