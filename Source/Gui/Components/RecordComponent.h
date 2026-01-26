#pragma once

#include <JuceHeader.h>

/// UI controls for starting and stopping recording.
class RecordComponent : public juce::Component {
public:
    /// Create the record controls.
    RecordComponent();

    /// Callback fired when recording starts.
    std::function<void()> onStartRecording;

    /// Callback fired when recording stops.
    std::function<void()> onStopRecording;

    /// Layout UI controls.
    void resized() override;

private:
    juce::TextButton startButton{ "Record" };
    juce::TextButton stopButton{ "Stop" };
};
