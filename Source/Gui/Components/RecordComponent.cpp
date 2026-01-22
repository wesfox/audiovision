#include "RecordComponent.h"

RecordComponent::RecordComponent()
{
    startButton.onClick = [this]() {
        if (onStartRecording) {
            onStartRecording();
        }
    };
    stopButton.onClick = [this]() {
        if (onStopRecording) {
            onStopRecording();
        }
    };
    addAndMakeVisible(startButton);
    addAndMakeVisible(stopButton);
}

void RecordComponent::resized()
{
    auto bounds = getLocalBounds();
    startButton.setBounds(bounds.removeFromLeft(120).reduced(4));
    stopButton.setBounds(bounds.removeFromLeft(120).reduced(4));
}
