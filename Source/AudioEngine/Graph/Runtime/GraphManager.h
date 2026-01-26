#pragma once

#include <JuceHeader.h>
#include <set>

#include "../Builder/GraphBuilder.h"
#include "GraphModule.h"
#include "Core/Track/AudioTrack.h"
#include "AudioEngine/Plugin/PluginChainBuilder.h"
#include "AudioEngine/Plugin/PluginInstanceFactory.h"
#include "AudioEngine/Plugin/PluginInstanceStore.h"
#include "AudioEngine/Parameters/ValueTreeManager.h"

class Track;
class Edit;
class GraphNode;
class RecordSession;

/// Build and own the runtime audio graph for an Edit.
class GraphManager {
public:
    /// Create a graph manager bound to an edit and graph.
    /// @param edit edit containing tracks and routing
    /// @param graph target audio processor graph
    /// @param recordSession recording session used by track nodes
    GraphManager(const std::weak_ptr<Edit>& edit,
                 const std::shared_ptr<AudioProcessorGraph>& graph,
                 RecordSession* recordSession);

    /// Build the graph description from the edit model.
    void createGraphFromEdit();

    ~GraphManager() = default;

    /// Source edit used for graph construction.
    const std::weak_ptr<Edit> edit;

    /// Graph nodes derived from the edit.
    std::vector<std::shared_ptr<GraphNode>> graphNodes;
    /// Runtime modules created for graph nodes.
    std::vector<std::unique_ptr<GraphModule>> graphModules;

    /// Find a graph node by track id.
    /// @param id track id
    GraphNode* getNodeById(String id);

    /// Find a graph module by graph node id.
    /// @param id graph node id
    GraphModule *getGraphModuleById(String id);

    /// Find a graph module by track id.
    /// @param id track id
    GraphModule *getGraphModuleByTrackId(String id);

    /// Build the final runtime graph (modules + connections).
    /// @param transport shared transport used by nodes
    void createFinalGraph(const std::shared_ptr<Transport>& transport);

    /// Attach the global audio output node.
    /// @param track output track target
    void attachAudioOutput(std::weak_ptr<Track> track);

    /// Prepare the graph for playback.
    /// @param sampleRate current engine sample rate
    /// @param blockSize current engine block size
    void prepareToPlay(double sampleRate, int blockSize) const;

    /// Underlying audio processor graph.
    std::shared_ptr<AudioProcessorGraph> graph;

    /// Graph description built from the edit.
    const GraphDescription& getGraphDescription() const { return graphDescription; }

    /// Update processing format without fully preparing the graph.
    /// @param sampleRate new sample rate
    /// @param blockSize new block size
    void setProcessingFormat(double sampleRate, int blockSize);

    /// Release resources and clear graph nodes.
    void shutdown();

    /// Find a plugin node by track id and plugin name.
    /// @param trackId track id owning the plugin
    /// @param pluginName plugin display name
    juce::AudioProcessorGraph::Node::Ptr findPluginNode(const String& trackId,
                                                        const String& pluginName) const;

    /// Find the first plugin node by plugin name.
    /// @param pluginName plugin display name
    juce::AudioProcessorGraph::Node::Ptr findPluginNodeByName(const String& pluginName) const;

    /// Access the parameter store for app-owned params.
    ValueTreeManager& getValueTreeManager() const { return *valueTreeManager; }

    /// Access the plugin instance store (graph-owned plugins).
    const PluginInstanceStore& getPluginInstanceStore() const { return pluginInstanceStore; }

private:
    /// Connect two modules for all channels in the format.
    /// @param inputModule upstream module
    /// @param outputModule downstream module
    /// @param format channel format to connect
    void buildConnection(const GraphModule* inputModule,
                         const GraphModule* outputModule,
                         ChannelsFormat format) const;

    juce::AudioProcessorGraph::Node::Ptr audioOutputNode;
    GraphBuilder graphBuilder;
    GraphDescription graphDescription;
    std::unique_ptr<PluginInstanceFactory> pluginFactory;
    PluginInstanceStore pluginInstanceStore;
    std::unique_ptr<PluginChainBuilder> pluginChainBuilder;
    RecordSession* recordSession = nullptr;
    std::unique_ptr<ValueTreeManager> valueTreeManager;
};
