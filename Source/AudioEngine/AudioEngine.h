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

/// Top-level audio engine coordinating graphs and output.
class AudioEngine{
public:
    /// Create an engine bound to an edit.
    /// @param edit edit containing tracks and routing
    AudioEngine(const std::weak_ptr<Edit>& edit);
    ~AudioEngine();

    /// Shared transport for playback/recording.
    std::shared_ptr<Transport> transport;
    /// Source edit for graph creation.
    std::weak_ptr<Edit> edit;
    /// Active graph instances (one per scene/session).
    std::vector<std::unique_ptr<GraphInstance>> graphInstances;

    /// Start playback.
    void start();

    /// Start recording in all graph instances.
    void startRecording();

    /// Stop recording in all graph instances.
    void stopRecording();

    /// Stop audio and release resources.
    void shutdown();

    /// Output engine for audio device handling.
    std::unique_ptr<AudioOutputEngine> audioOutputEngine;

    /// Access the shared transport.
    std::shared_ptr<Transport> getTransport() const {
        return transport;
    }

    /// Access active graph instances.
    const std::vector<std::unique_ptr<GraphInstance>>& getGraphInstances() const {
        return graphInstances;
    }

    /// Find a plugin node by track id and name.
    /// @param trackId track id owning the plugin
    /// @param pluginName plugin display name
    juce::AudioProcessorGraph::Node::Ptr getPluginNode(const String& trackId,
                                                       const String& pluginName) const;

    /// Find the first plugin node by name.
    /// @param pluginName plugin display name
    juce::AudioProcessorGraph::Node::Ptr getPluginNodeByName(const String& pluginName) const;

    /// Access the plugin instance store (graph-owned plugins).
    const PluginInstanceStore* getPluginInstanceStore() const;

    /// Access the audio output engine.
    AudioOutputEngine& getAudioOutputEngine() const {
        return *audioOutputEngine;
    }

private:
    std::unique_ptr<PluginInstanceFactory> pluginHost;
    std::unique_ptr<PluginRegistry> pluginRegistry;

};
