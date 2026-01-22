#include "Recorder.h"

Recorder::Recorder(int numChannels, int64 maxSamples, double sampleRate)
    : recordingBuffer(numChannels, static_cast<int>(maxSamples)),
      writePosition(0),
      sampleRate(sampleRate)
{
    recordingBuffer.clear();
}

void Recorder::pushBuffer(const juce::AudioBuffer<float>& buffer, int numSamples)
{
    const int available = recordingBuffer.getNumSamples() - static_cast<int>(writePosition);
    if (available <= 0) {
        return;
    }
    const int samplesToCopy = std::min(available, numSamples);
    const int channels = std::min(recordingBuffer.getNumChannels(), buffer.getNumChannels());
    for (int channel = 0; channel < channels; ++channel) {
        recordingBuffer.copyFrom(channel, static_cast<int>(writePosition), buffer, channel, 0, samplesToCopy);
    }
    writePosition += samplesToCopy;
}

void Recorder::writeToFile(const juce::File& file) const
{
    if (writePosition <= 0) {
        return;
    }
    if (auto stream = std::unique_ptr<juce::FileOutputStream>(file.createOutputStream())) {
        juce::WavAudioFormat format;
        auto writer = std::unique_ptr<juce::AudioFormatWriter>(
            format.createWriterFor(stream.get(), sampleRate,
                                   static_cast<unsigned int>(recordingBuffer.getNumChannels()),
                                   16, {}, 0));
        if (writer != nullptr) {
            stream.release();
            writer->writeFromAudioSampleBuffer(recordingBuffer, 0, static_cast<int>(writePosition));
        }
    }
}
