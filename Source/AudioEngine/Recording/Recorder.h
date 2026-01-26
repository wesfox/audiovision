#pragma once

#include <JuceHeader.h>

/// In-memory audio recorder used during a record pass.
class Recorder {
public:
    Recorder(int numChannels, int64 maxSamples, double sampleRate);

    /// Append samples from an incoming buffer.
    /// @param buffer source audio buffer
    /// @param numSamples number of samples to copy from buffer
    void pushBuffer(const juce::AudioBuffer<float>& buffer, int numSamples);

    /// Total number of samples captured so far.
    int64 getNumSamplesRecorded() const { return writePosition; }

    /// Channel count of the internal recording buffer.
    int getNumChannels() const { return recordingBuffer.getNumChannels(); }

    /// Sample rate used by the recording buffer.
    double getSampleRate() const { return sampleRate; }

    /// Flush the recorded data to an audio file.
    /// @param file destination file
    void writeToFile(const juce::File& file) const;

private:
    juce::AudioBuffer<float> recordingBuffer;
    int64 writePosition = 0;
    double sampleRate = 0.0;
};
