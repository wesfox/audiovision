#pragma once

#include <JuceHeader.h>

#include "AudioOutputManager.h"
#include "Utils/Format.h"

#include <vector>

class GraphInstance;
class Transport;

/// Own audio device setup and connect the graph to the output.
class AudioOutputEngine {
public:
    /// Create an output engine for a set of graph instances.
    /// @param graphInstances graphs to render
    /// @param transport shared transport for playback state
    AudioOutputEngine(std::vector<std::unique_ptr<GraphInstance>>& graphInstances,
                      const std::weak_ptr<Transport>& transport);
    ~AudioOutputEngine();

    /// Initialise the output device.
    /// @param format channel format to use
    /// @param rate sample rate
    /// @param size block size
    void initialise(ChannelsFormat format, double rate, int size);

    /// Stop and release the output device.
    void shutdown();

    /// True when the output device is ready.
    [[nodiscard]] bool isInitialised() const noexcept { return initialised; }

    /// Access the output manager for callbacks.
    [[nodiscard]] AudioOutputManager& getOutputManager() noexcept { return audioOutputManager; }

    /// Access the underlying device manager.
    [[nodiscard]] juce::AudioDeviceManager& getDeviceManager() noexcept { return deviceManager; }

private:
    /// Configure device settings before opening.
    /// @param format channel format to use
    /// @param rate sample rate
    /// @param size block size
    void configureDevice(ChannelsFormat format, double rate, int size);

    juce::AudioDeviceManager deviceManager;
    AudioOutputManager audioOutputManager;
    bool initialised = false;
};
