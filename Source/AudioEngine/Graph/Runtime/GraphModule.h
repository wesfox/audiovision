#pragma once

#include <JuceHeader.h>

#include "../Model/GraphNode.h"
#include "Core/Edit/Edit.h"
#include "Core/Track/AudioTrack.h"
#include "Utils/Format.h"

class PluginChainBuilder;
class RecordSession;
class ValueTreeManager;

/// Runtime module wiring nodes and plugins for a single graph node.
class GraphModule{
public:
    /// Create a module for a graph node.
    /// @param graphNode model node to instantiate
    /// @param graph target audio processor graph
    /// @param edit edit containing tracks and routing
    /// @param transport shared transport used by nodes
    /// @param pluginChainBuilder plugin chain builder for this module
    /// @param recordSession recording session used by track nodes
    /// @param valueTreeManager parameter store for app-owned params
    explicit GraphModule(GraphNode * graphNode,
                         const std::weak_ptr<AudioProcessorGraph>& graph,
                         const std::weak_ptr<Edit>& edit,
                         const std::weak_ptr<Transport>& transport,
                         PluginChainBuilder* pluginChainBuilder,
                         RecordSession* recordSession,
                         ValueTreeManager* valueTreeManager);

    /// Find an audio track by id (returns empty if missing).
    /// @param trackId track id to search
    [[nodiscard]] std::weak_ptr<AudioTrack> getAudioTrackById(const String &trackId) const;

    /// Find any track by id (returns empty if missing).
    /// @param trackId track id to search
    std::shared_ptr<Track> getTrackById(const String& trackId) const;

    /// First node in this module's processing chain.
    juce::AudioProcessorGraph::Node::Ptr inputNode;
    /// Last node in this module's processing chain.
    juce::AudioProcessorGraph::Node::Ptr outputNode;

    /// Model node associated with this module.
    GraphNode* virtualGraphNode;
    /// Edit used for track lookups.
    std::weak_ptr<Edit> edit;

    /// Transport shared by runtime nodes.
    std::weak_ptr<Transport> transport;
private:
    /// Access the underlying audio graph (throws if missing).
    AudioProcessorGraph* getGraphRef() const;

    /// Connect two nodes for all channels in the format.
    /// @param graph target graph
    /// @param nodeInput upstream node
    /// @param nodeOutput downstream node
    /// @param format channel format to connect
    static void connectNodes(AudioProcessorGraph& graph,
                             const AudioProcessorGraph::Node* nodeInput,
                             const AudioProcessorGraph::Node* nodeOutput,
                             ChannelsFormat format);

    std::weak_ptr<AudioProcessorGraph> graph;
    PluginChainBuilder* pluginChainBuilder;
    RecordSession* recordSession;
    ValueTreeManager* valueTreeManager = nullptr;
};
