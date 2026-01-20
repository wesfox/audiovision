#pragma once

#include <JuceHeader.h>

#include "Utils/Format.h"

class AudioEngine;

class AudioOutputManager : public juce::Component, public juce::AudioIODeviceCallback {
public:
    AudioOutputManager(const std::weak_ptr<juce::AudioProcessorGraph> &graph, AudioEngine* audioEngine);
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
    AudioEngine* audioEngine;
    std::weak_ptr<juce::AudioProcessorGraph> graph;
    ChannelsFormat channelsFormat = ChannelsFormat::Mono;
    double sampleRate = 48000.0;
    int blockSize = 512;
};
