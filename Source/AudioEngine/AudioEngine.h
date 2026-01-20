#pragma once

#include <JuceHeader.h>

#include "AudioOutput/AudioOutputEngine.h"
#include "Graph/Runtime/GraphManager.h"
#include "Utils/Transport.h"

class Edit;

class AudioEngine{
public:
    AudioEngine(const std::weak_ptr<Edit>& edit);

    std::shared_ptr<Transport> transport;
    std::weak_ptr<Edit> edit;
    std::unique_ptr<GraphManager> graphManager;

    void start();

    std::shared_ptr<juce::AudioProcessorGraph> audioGraph;
    std::unique_ptr<AudioOutputEngine> audioOutputEngine;

    std::shared_ptr<juce::AudioProcessorGraph> getAudioGraph() const {
        return audioGraph;
    }

    std::shared_ptr<Transport> getTransport() const {
        return transport;
    }

    AudioOutputEngine& getAudioOutputEngine() const {
        return *audioOutputEngine;
    }

private:

};
