#pragma once

#include <JuceHeader.h>

#include "AudioOutputManager.h"
#include "Utils/Format.h"

class AudioOutputEngine {
public:
    explicit AudioOutputEngine(std::shared_ptr<juce::AudioProcessorGraph> graph);
    ~AudioOutputEngine();

    void initialise(ChannelsFormat format, double rate, int size);
    void shutdown();

    [[nodiscard]] bool isInitialised() const noexcept { return initialised; }
    [[nodiscard]] AudioOutputManager& getOutputManager() noexcept { return audioOutputManager; }
    [[nodiscard]] juce::AudioDeviceManager& getDeviceManager() noexcept { return deviceManager; }

private:
    void configureDevice(ChannelsFormat format, double rate, int size);

    std::shared_ptr<juce::AudioProcessorGraph> graph;
    juce::AudioDeviceManager deviceManager;
    AudioOutputManager audioOutputManager;
    bool initialised = false;
};
