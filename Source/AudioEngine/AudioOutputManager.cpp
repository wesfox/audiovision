#include "AudioOutputManager.h"

// ------------------------ MainComponent Implementation ------------------------

AudioOutputManager::AudioOutputManager(const std::weak_ptr<juce::AudioProcessorGraph> &graph_) {
    graph = graph_;
}

void AudioOutputManager::configure(ChannelsFormat format, double rate, int size) noexcept
{
    channelsFormat = format;
    sampleRate = rate;
    blockSize = size;
}

void AudioOutputManager::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    if (device != nullptr) {
        sampleRate = device->getCurrentSampleRate();
        blockSize = device->getCurrentBufferSizeSamples();
    }
}

void AudioOutputManager::audioDeviceStopped()
{
}

void AudioOutputManager::audioDeviceIOCallbackWithContext(
    const float *const*inputChannelData,
    int,
    float *const*outputChannelData,
    int numOutputChannels, int numSamples, const AudioIODeviceCallbackContext &context
    )
{
    for (int channel = 0; channel < numOutputChannels; ++channel) {
        if (auto* output = outputChannelData[channel]) {
            juce::FloatVectorOperations::clear(output, numSamples);
        }
    }
    juce::AudioBuffer<float> buffer(numOutputChannels, numSamples);

    juce::MidiBuffer midi;

    auto start = std::chrono::high_resolution_clock::now();
    graph.lock()->processBlock(buffer, midi);

    for (int channel = 0; channel < numOutputChannels; ++channel) {
        for (int sample = 0; sample < numSamples; ++sample) {
            buffer.setSample(channel, sample, 0);
        }
    }
}
