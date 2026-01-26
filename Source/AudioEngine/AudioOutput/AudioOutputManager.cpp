#include "AudioOutputManager.h"

#include "../Graph/Runtime/GraphInstance.h"
#include "Utils/Transport.h"

// ------------------------ MainComponent Implementation ------------------------

AudioOutputManager::AudioOutputManager(std::vector<std::unique_ptr<GraphInstance>>& graphInstances,
                                       const std::weak_ptr<Transport>& transport)
    : graphInstances(graphInstances),
      transport(transport)
{
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
    if (auto transportPtr = transport.lock()) {
        transportPtr->prepare(sampleRate);
        transportPtr->setCurrentBlockSize(blockSize);
    }
    for (auto& instance : graphInstances) {
        if (instance) {
            instance->prepareToPlay(sampleRate, blockSize);
        }
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
    mixBuffer.setSize(numOutputChannels, numSamples, false, false, true);
    mixBuffer.clear();
    tempBuffer.setSize(numOutputChannels, numSamples, false, false, true);

    juce::MidiBuffer midi;

    for (const auto& instance : graphInstances) {
        if (!instance) {
            continue;
        }
        tempBuffer.clear();
        if (const auto transportPtr = transport.lock()) {
            if (transportPtr->isPlaying())
                instance->processBlock(tempBuffer, midi);
        }
        for (int channel = 0; channel < numOutputChannels; ++channel) {
            mixBuffer.addFrom(channel, 0, tempBuffer, channel, 0, numSamples);
        }
    }

    if (const auto transportPtr = transport.lock()) {
        if (transportPtr->isPlaying())
            transportPtr->advance(numSamples);
    }

    for (int channel = 0; channel < numOutputChannels; ++channel) {
        if (auto* output = outputChannelData[channel]) {
            juce::FloatVectorOperations::copy(output, mixBuffer.getReadPointer(channel), numSamples);
        }
    }
}
