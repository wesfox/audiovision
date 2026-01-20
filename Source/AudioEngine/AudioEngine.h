#pragma once

#include <JuceHeader.h>

#include "AudioOutputEngine.h"
#include "Graph/GraphManager.h"
#include "Utils/Transport.h"

class Edit;

class AudioEngine{
public:
    AudioEngine(const std::weak_ptr<Edit>& edit);

    Transport transport = Transport();
    std::weak_ptr<Edit> edit;
    std::unique_ptr<GraphManager> graphManager;

    void start();

    std::shared_ptr<juce::AudioProcessorGraph> audioGraph;
    std::unique_ptr<AudioOutputEngine> audioOutputEngine;

    std::shared_ptr<juce::AudioProcessorGraph> getAudioGraph() const {
        return audioGraph;
    }

    AudioOutputEngine& getAudioOutputEngine() const {
        return *audioOutputEngine;
    }

private:

};
