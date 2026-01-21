#pragma once

#include <JuceHeader.h>

#include "AudioOutput/AudioOutputEngine.h"
#include "Graph/Runtime/GraphInstance.h"
#include "Utils/Transport.h"

#include <vector>

class Edit;

class AudioEngine{
public:
    AudioEngine(const std::weak_ptr<Edit>& edit);

    std::shared_ptr<Transport> transport;
    std::weak_ptr<Edit> edit;
    std::vector<std::unique_ptr<GraphInstance>> graphInstances;

    void start();

    std::unique_ptr<AudioOutputEngine> audioOutputEngine;

    std::shared_ptr<Transport> getTransport() const {
        return transport;
    }

    const std::vector<std::unique_ptr<GraphInstance>>& getGraphInstances() const {
        return graphInstances;
    }

    AudioOutputEngine& getAudioOutputEngine() const {
        return *audioOutputEngine;
    }

private:

};
