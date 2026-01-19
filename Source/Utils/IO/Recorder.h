#pragma once

#include <JuceHeader.h>

class Recorder {
public:
    Recorder();

    void save();

    void record(AudioBuffer<float> &buffer);

    std::vector<AudioBuffer<float>> fullBuffer;

    juce::File outFile = juce::File::getCurrentWorkingDirectory()
                            .getChildFile("render.wav");
    std::unique_ptr<OutputStream> outputStream;

    juce::WavAudioFormat format;

    std::unique_ptr<juce::AudioFormatWriter> writer;

    private:
};