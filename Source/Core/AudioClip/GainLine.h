#pragma once

#include <JuceHeader.h>

/// Gain automation curve for an audio clip.
class GainLine : public juce::Component {
public:
    /// Create an empty gain line.
    GainLine();

    /// Split the gain line when splitting a clip.
    /// @param splitSample absolute split position in timeline samples
    std::unique_ptr<GainLine> split(int64 splitSample);

private:
};
