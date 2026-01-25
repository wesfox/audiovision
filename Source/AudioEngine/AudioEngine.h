#pragma once

#include <JuceHeader.h>

#include "AudioOutput/AudioOutputEngine.h"
#include "Plugin/PluginInstanceFactory.h"
#include "Graph/Runtime/GraphInstance.h"
#include "Utils/Transport.h"

#include <vector>

class Edit;
class PluginRegistry;
class PluginInstanceStore;

class AudioEngine{
public:
    AudioEngine(const std::weak_ptr<Edit>& edit);
    ~AudioEngine();

    std::shared_ptr<Transport> transport;
    std::weak_ptr<Edit> edit;
    std::vector<std::unique_ptr<GraphInstance>> graphInstances;

    void start();
    void startRecording();
    void stopRecording();
    void shutdown();

    std::unique_ptr<AudioOutputEngine> audioOutputEngine;

    std::shared_ptr<Transport> getTransport() const {
        return transport;
    }

    const std::vector<std::unique_ptr<GraphInstance>>& getGraphInstances() const {
        return graphInstances;
    }

    juce::AudioProcessorGraph::Node::Ptr getPluginNode(const String& trackId,
                                                       const String& pluginName) const;
    juce::AudioProcessorGraph::Node::Ptr getPluginNodeByName(const String& pluginName) const;
    const PluginInstanceStore* getPluginInstanceStore() const;

    AudioOutputEngine& getAudioOutputEngine() const {
        return *audioOutputEngine;
    }

private:
    std::unique_ptr<PluginInstanceFactory> pluginHost;
    std::unique_ptr<PluginRegistry> pluginRegistry;

};
