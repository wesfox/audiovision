#pragma once

#include <JuceHeader.h>

#include "../Model/GraphNode.h"
#include "Core/Edit/Edit.h"
#include "Core/Track/AudioTrack.h"
#include "Utils/Format.h"

class PluginChainBuilder;

class GraphModule{
public:
    explicit GraphModule(GraphNode * graphNode,
                         const std::weak_ptr<AudioProcessorGraph>& graph,
                         const std::weak_ptr<Edit>& edit,
                         const std::weak_ptr<Transport>& transport,
                         PluginChainBuilder* pluginChainBuilder);

    [[nodiscard]] std::weak_ptr<AudioTrack> getAudioTrackById(const String &trackId) const;
    std::shared_ptr<Track> getTrackById(const String& trackId) const;

    juce::AudioProcessorGraph::Node::Ptr inputNode;
    juce::AudioProcessorGraph::Node::Ptr outputNode;

    GraphNode* virtualGraphNode;
    std::weak_ptr<Edit> edit;

    std::weak_ptr<Transport> transport;
private:
    AudioProcessorGraph* getGraphRef() const;
    static void connectNodes(AudioProcessorGraph& graph,
                             const AudioProcessorGraph::Node* nodeInput,
                             const AudioProcessorGraph::Node* nodeOutput,
                             ChannelsFormat format);

    std::weak_ptr<AudioProcessorGraph> graph;
    PluginChainBuilder* pluginChainBuilder;
};
