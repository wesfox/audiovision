#pragma once

#include <JuceHeader.h>

class RecordComponent : public juce::Component {
public:
    RecordComponent();

    std::function<void()> onStartRecording;
    std::function<void()> onStopRecording;

    void resized() override;

private:
    juce::TextButton startButton{ "Record" };
    juce::TextButton stopButton{ "Stop" };
};
