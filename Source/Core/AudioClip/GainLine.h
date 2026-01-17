#pragma once

#include <JuceHeader.h>

class GainLine : public juce::Component {
public:
    GainLine();

    /// allows to split a GainLine (used when splitting a Clip), returns a coherent GainLine for the new Clip
    /// @param splitSample absolute splitSample (relative to the timeline)
    std::unique_ptr<GainLine> split(int64 splitSample);

private:
};