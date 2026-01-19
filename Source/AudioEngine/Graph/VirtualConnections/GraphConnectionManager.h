#pragma once

#include <JuceHeader.h>

#include "GraphVirtualConnection.h"
#include "AudioEngine/Graph/GraphModule.h"

class GraphConnectionManager {
public:
    GraphConnectionManager(std::vector<std::shared_ptr<GraphNode>>& graphNodes, std::weak_ptr<AudioProcessorGraph> graph);

    void buildConnection(const GraphModule *moduleInput, const GraphModule *moduleOutput, ChannelsFormat format) const;

    void buildGraphVirtualConnections();

    void addVirtualConnectionsFromNode(GraphNode *node, std::set<std::pair<String, String>> &seenConnections,
                                 std::set<String> &visitedNodes);

    void addUniqueVirtualConnection(GraphNode *input, GraphNode *output, std::set<std::pair<String, String>> &seenConnections);

    std::vector<std::unique_ptr<GraphVirtualConnection>> graphVirtualConnections;

private:
    std::vector<std::shared_ptr<GraphNode>>& graphNodes;
    std::weak_ptr<AudioProcessorGraph> graph;

    [[nodiscard]] AudioProcessorGraph* getGraphRef() const {
        auto graphRef = graph.lock().get();
        if (graphRef == nullptr) {
            throw std::runtime_error("GraphNode is nullptr");
        }
        return graphRef;
    }

};
