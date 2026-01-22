#pragma once

#include <JuceHeader.h>

#include "AudioOutput/AudioOutputEngine.h"
#include "Plugin/PluginInstanceFactory.h"
#include "Graph/Runtime/GraphInstance.h"
#include "Utils/Transport.h"

#include <vector>

class Edit;
class PluginRegistry;

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

    /// Create a pluginRuntime to show it, Exemple Code :
    /// String reverbTrackId;
    /// for (const auto& track : edit->getTracks()) {
    ///  if (track->getName() == "reverbAuxTrack") {
    ///     reverbTrackId = track->getId();
    ///     break;
    /// }
    /// }
    /// if (reverbTrackId.isNotEmpty()) {
    ///     auto node = audioEngine->getPluginNode(reverbTrackId, "TAL-Reverb-2");
    ///     if (node != nullptr) {
    ///         pluginEditorWindow->setPluginNode(node);
    ///         pluginEditorWindow->setVisible(true);
    ///     } else {
    ///         juce::Logger::writeToLog("Plugin node not found for reverbAuxTrack");
    ///     }
    /// } else {
    ///     juce::Logger::writeToLog("Reverb aux track not found");
    /// }
    std::shared_ptr<PluginRuntime> createPluginRuntimeByName(const String& name,
                                                             double sampleRate,
                                                             int blockSize,
                                                             juce::String& error) const;
    juce::AudioProcessorGraph::Node::Ptr getPluginNode(const String& trackId,
                                                       const String& pluginName) const;

    AudioOutputEngine& getAudioOutputEngine() const {
        return *audioOutputEngine;
    }

private:
    std::unique_ptr<PluginInstanceFactory> pluginHost;
    std::unique_ptr<PluginRegistry> pluginRegistry;

};
