#pragma once

#include <JuceHeader.h>

#include "GraphNode.h"
#include "Utils/Format.h"

class GraphModule{
public:
    explicit GraphModule(GraphNode * graphNode, const std::weak_ptr<AudioProcessorGraph>& graph);

    void buildInternalConnexion(const AudioProcessorGraph::Node *nodeInput, const AudioProcessorGraph::Node *nodeOutput,
                        ChannelsFormat format) const;

    juce::AudioProcessorGraph::Node::Ptr inputNode;
    juce::AudioProcessorGraph::Node::Ptr outputNode;

    GraphNode* virtualGraphNode;

private:
    std::weak_ptr<AudioProcessorGraph> graph;
    std::vector<std::unique_ptr<AudioProcessor>> audioProcessors;
};
