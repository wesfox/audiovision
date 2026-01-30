#include "Recorder.h"

// ------------------------ MainComponent Implementation ------------------------

Recorder::Recorder() {
    outFile = juce::File("~/code/TestLundi").getChildFile("records")
                            .getChildFile("render.wav");

    outputStream = outFile.createOutputStream();

    auto options = juce::AudioFormatWriterOptions()
                       .withSampleRate(44100.0)
                       .withNumChannels(2)
                       .withBitsPerSample(16)
                       .withMetadataValues({});
    writer = format.createWriterFor(outputStream, options);
}

void Recorder::record(AudioBuffer<float>& buffer) {

    fullBuffer.emplace_back(buffer);
}

void Recorder::save() {
    for (auto& buffer : fullBuffer) {
        writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
    }
    outputStream.release();
}
