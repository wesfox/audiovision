#pragma once

#include <JuceHeader.h>

#include "GraphNode.h"
#include "Core/Edit/Edit.h"
#include "Core/Track/AudioTrack.h"
#include "Utils/Format.h"

class GraphModule{
public:
    explicit GraphModule(GraphNode * graphNode, const std::weak_ptr<AudioProcessorGraph>& graph, const std::weak_ptr<Edit>& edit, const Transport* transport);

    void buildInternalConnexion(const AudioProcessorGraph::Node *nodeInput, const AudioProcessorGraph::Node *nodeOutput,
                        ChannelsFormat format) const;

    [[nodiscard]] std::weak_ptr<AudioTrack> getAudioTrackById(const String &trackId) const;

    juce::AudioProcessorGraph::Node::Ptr inputNode;
    juce::AudioProcessorGraph::Node::Ptr outputNode;

    GraphNode* virtualGraphNode;
    std::weak_ptr<Edit> edit;

    const Transport *transport;
private:
    std::weak_ptr<AudioProcessorGraph> graph;
    std::vector<std::unique_ptr<AudioProcessor>> audioProcessors;
};
