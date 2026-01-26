#pragma once

#include <JuceHeader.h>

/// Simple buffer recorder for file rendering.
class Recorder {
public:
    /// Create a recorder with default output path.
    Recorder();

    /// Write the recorded buffer to disk.
    void save();

    /// Append audio to the recording buffer.
    /// @param buffer audio buffer to record
    void record(AudioBuffer<float> &buffer);

    /// Accumulated buffers for rendering.
    std::vector<AudioBuffer<float>> fullBuffer;

    /// Output file path for rendering.
    juce::File outFile = juce::File::getCurrentWorkingDirectory()
                            .getChildFile("render.wav");
    /// Output stream for file writing.
    std::unique_ptr<OutputStream> outputStream;

    /// Wav file format used for writing.
    juce::WavAudioFormat format;

    /// Writer used to serialize audio to disk.
    std::unique_ptr<juce::AudioFormatWriter> writer;

    private:
};
