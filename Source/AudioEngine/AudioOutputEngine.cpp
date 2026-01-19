#include "AudioOutputEngine.h"

AudioOutputEngine::AudioOutputEngine(std::shared_ptr<juce::AudioProcessorGraph> graph_)
    : graph(std::move(graph_)),
      audioOutputManager(graph)
{
}

AudioOutputEngine::~AudioOutputEngine()
{
    shutdown();
}

void AudioOutputEngine::initialise(ChannelsFormat format, double rate, int size)
{
    if (initialised) {
        return;
    }

    audioOutputManager.configure(format, rate, size);
    configureDevice(format, rate, size);
    deviceManager.addAudioCallback(&audioOutputManager);
    initialised = true;
}

void AudioOutputEngine::shutdown()
{
    if (!initialised) {
        return;
    }

    deviceManager.removeAudioCallback(&audioOutputManager);
    deviceManager.closeAudioDevice();
    initialised = false;
}

void AudioOutputEngine::configureDevice(ChannelsFormat format, double rate, int size)
{
    const int outputChannels = ChannelCount(format);
    auto initResult = deviceManager.initialise(0, outputChannels, nullptr, true);
    if (initResult.isNotEmpty()) {
        juce::Logger::writeToLog("Audio device init error: " + initResult);
        return;
    }

    juce::AudioDeviceManager::AudioDeviceSetup setup;
    deviceManager.getAudioDeviceSetup(setup);
    setup.sampleRate = rate;
    setup.bufferSize = size;
    setup.inputChannels.clear();
    setup.outputChannels.clear();
    setup.outputChannels.setRange(0, outputChannels, true);
    auto setupResult = deviceManager.setAudioDeviceSetup(setup, true);
    if (setupResult.isNotEmpty()) {
        juce::Logger::writeToLog("Audio device setup error: " + setupResult);
    }
}
