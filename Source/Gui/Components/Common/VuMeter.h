#pragma once

#include <JuceHeader.h>

/// Displays a simple VU meter with a colored fill.
class VuMeter : public juce::Component {
public:
    VuMeter();

    void paint(juce::Graphics& g) override;

    void setVolume(float newVolume);

private:
    float volume = 0.0f;
};
