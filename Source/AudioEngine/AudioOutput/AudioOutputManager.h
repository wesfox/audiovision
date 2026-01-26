#pragma once

#include <JuceHeader.h>

#include "Utils/Format.h"

#include <memory>
#include <vector>

class GraphInstance;
class Transport;

/// Audio device callback that mixes graph instances to the output.
class AudioOutputManager : public juce::Component, public juce::AudioIODeviceCallback {
public:
    /// Create an output manager for graph instances.
    /// @param graphInstances graphs to render
    /// @param transport shared transport for playback state
    AudioOutputManager(std::vector<std::unique_ptr<GraphInstance>>& graphInstances,
                       const std::weak_ptr<Transport>& transport);
    ~AudioOutputManager() override = default;

    /// Configure output format and buffer sizes.
    /// @param format channel format to use
    /// @param rate sample rate
    /// @param size block size
    void configure(ChannelsFormat format, double rate, int size) noexcept;

    /// Render audio into the output buffers.
    void audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                           int numInputChannels,
                                           float* const* outputChannelData,
                                           int numOutputChannels,
                                           int numSamples,
                                           const AudioIODeviceCallbackContext& context) override;

    /// Prepare audio device resources.
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;

    /// Release audio device resources.
    void audioDeviceStopped() override;

    /// Current channel format for output.
    [[nodiscard]] ChannelsFormat getChannelsFormat() const noexcept { return channelsFormat; }

    /// Current sample rate.
    [[nodiscard]] double getSampleRate() const noexcept { return sampleRate; }

    /// Current block size.
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
