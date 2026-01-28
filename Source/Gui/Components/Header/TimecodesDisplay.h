#pragma once

#include <JuceHeader.h>

/// Renders the header timecode readouts.
class TimecodesDisplay : public juce::Component {
public:
    TimecodesDisplay();

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
    juce::String timecode = "01:15:21:05";
    juce::String inTimecode = "01:15:21:05";
    juce::String outTimecode = "01:15:22:06";
    juce::String lengthTimecode = "00:00:01:01";
};
