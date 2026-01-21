#pragma once

#include <JuceHeader.h>

#include "AudioOutputManager.h"
#include "Utils/Format.h"

#include <vector>

class GraphInstance;
class Transport;

class AudioOutputEngine {
public:
    AudioOutputEngine(std::vector<std::unique_ptr<GraphInstance>>& graphInstances,
                      const std::weak_ptr<Transport>& transport);
    ~AudioOutputEngine();

    void initialise(ChannelsFormat format, double rate, int size);
    void shutdown();

    [[nodiscard]] bool isInitialised() const noexcept { return initialised; }
    [[nodiscard]] AudioOutputManager& getOutputManager() noexcept { return audioOutputManager; }
    [[nodiscard]] juce::AudioDeviceManager& getDeviceManager() noexcept { return deviceManager; }

private:
    void configureDevice(ChannelsFormat format, double rate, int size);

    juce::AudioDeviceManager deviceManager;
    AudioOutputManager audioOutputManager;
    bool initialised = false;
};
