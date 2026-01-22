#pragma once

#include <JuceHeader.h>

class Recorder {
public:
    Recorder(int numChannels, int64 maxSamples, double sampleRate);

    void pushBuffer(const juce::AudioBuffer<float>& buffer, int numSamples);
    int64 getNumSamplesRecorded() const { return writePosition; }
    int getNumChannels() const { return recordingBuffer.getNumChannels(); }
    double getSampleRate() const { return sampleRate; }

    void writeToFile(const juce::File& file) const;

private:
    juce::AudioBuffer<float> recordingBuffer;
    int64 writePosition = 0;
    double sampleRate = 0.0;
};
