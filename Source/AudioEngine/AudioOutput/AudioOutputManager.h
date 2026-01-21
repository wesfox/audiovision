#pragma once

#include <JuceHeader.h>

#include "Utils/Format.h"

#include <memory>
#include <vector>

class GraphInstance;
class Transport;

class AudioOutputManager : public juce::Component, public juce::AudioIODeviceCallback {
public:
    AudioOutputManager(std::vector<std::unique_ptr<GraphInstance>>& graphInstances,
                       const std::weak_ptr<Transport>& transport);
    ~AudioOutputManager() override = default;

    void configure(ChannelsFormat format, double rate, int size) noexcept;

    void audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                           int numInputChannels,
                                           float* const* outputChannelData,
                                           int numOutputChannels,
                                           int numSamples,
                                           const AudioIODeviceCallbackContext& context) override;
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

    [[nodiscard]] ChannelsFormat getChannelsFormat() const noexcept { return channelsFormat; }
    [[nodiscard]] double getSampleRate() const noexcept { return sampleRate; }
    [[nodiscard]] int getBlockSize() const noexcept { return blockSize; }

private:
    std::vector<std::unique_ptr<GraphInstance>>& graphInstances;
    std::weak_ptr<Transport> transport;
    ChannelsFormat channelsFormat = ChannelsFormat::Mono;
    double sampleRate = 48000.0;
    int blockSize = 512;
    juce::AudioBuffer<float> mixBuffer;
    juce::AudioBuffer<float> tempBuffer;
};
