#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"

/// Renders the header timecode readouts.
class TimecodesDisplay : public juce::Component,
                         private juce::Timer {
public:
    explicit TimecodesDisplay(Edit& edit);

    void paint(juce::Graphics& g) override;

    void setTimecode(juce::String newTimecode);
    void setInTimecode(juce::String newTimecode);
    void setOutTimecode(juce::String newTimecode);
    void setLengthTimecode(juce::String newTimecode);

    static constexpr int kHeight = 56;
    static constexpr int kLeftWidth = 150;
    static constexpr int kRightWidth = 120;
    static constexpr float kCornerRadius = 4.0f;
    static constexpr int kLabelWidth = 40;
    static constexpr int kWidth = kLeftWidth + kRightWidth;

private:
    void timerCallback() override;
    void updateTimecodes(bool force);

    Edit& edit;
    int64 playheadFrame = 0;
    int64 inFrame = 0;
    int64 outFrame = 0;

    juce::String timecode = "00:00:00:00";
    juce::String inTimecode = "01:15:21:05";
    juce::String outTimecode = "01:15:22:06";
    juce::String lengthTimecode = "00:00:01:01";
};
