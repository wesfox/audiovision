#include "Recorder.h"

// ------------------------ MainComponent Implementation ------------------------

Recorder::Recorder() {
    outFile = juce::File("~/code/TestLundi").getChildFile("records")
                            .getChildFile("render.wav");

    outputStream = outFile.createOutputStream();

    juce::WavAudioFormat wavFormat;
    writer.reset(wavFormat.createWriterFor(
        outputStream.get(),
        44100.0,
        2,
        16,
        {},
        0));
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
