#include "GraphConnectionManager.h"

#include <utility>

#include "../GraphNode.h"
#include "AudioEngine/Graph/GraphModule.h"

// ------------------------ MainComponent Implementation ------------------------

GraphConnectionManager::GraphConnectionManager(std::vector<std::shared_ptr<GraphNode>>& graphNodes, std::weak_ptr<AudioProcessorGraph> graph):graphNodes(graphNodes), graph(std::move(graph)) {

}

/// must be called for each connection to really create between modules
void GraphConnectionManager::buildConnection(const GraphModule* moduleInput, const GraphModule* moduleOutput, const ChannelsFormat format) const {
    const auto graphRef = getGraphRef();
    for (auto i=0; i<ChannelCount(format); i++) {
        graphRef->addConnection({
            { moduleInput->inputNode->nodeID, i },
            { moduleOutput->inputNode->nodeID, i }
        });
    }
}

/// This function must be called in order to build the GraphConnections
void GraphConnectionManager::buildGraphVirtualConnections()
{
    graphVirtualConnections.clear();
    std::set<std::pair<String, String>> seenConnections;
    std::set<String> visitedNodes;

    for (const auto& node : graphNodes) {
        if (node->isGraphStart) {
            addVirtualConnectionsFromNode(node.get(), seenConnections, visitedNodes);
        }
    }
}

void GraphConnectionManager::addVirtualConnectionsFromNode(
    GraphNode *node,
    std::set<std::pair<String, String>>& seenConnections,
    std::set<String>& visitedNodes)
{
    if (node == nullptr) {
        return;
    }

    auto nodeId = node->getId();
    if (visitedNodes.find(nodeId) != visitedNodes.end()) {
        return;
    }
    visitedNodes.insert(nodeId);

    for (auto send : node->sends) {
        addUniqueVirtualConnection(node, send, seenConnections);
        addVirtualConnectionsFromNode(send, seenConnections, visitedNodes);
    }

    if (node->output != nullptr) {
        addUniqueVirtualConnection(node, node->output, seenConnections);
        addVirtualConnectionsFromNode(node->output, seenConnections, visitedNodes);
    }
}

void GraphConnectionManager::addUniqueVirtualConnection(
    GraphNode *input,
    GraphNode *output,
    std::set<std::pair<String, String>>& seenConnections)
{
    if (input == nullptr || output == nullptr) {
        return;
    }

    std::pair<String, String> key { input->getId(), output->getId() };
    if (seenConnections.insert(key).second) {
        graphVirtualConnections.push_back(std::make_unique<GraphVirtualConnection>(input, output));
    }
}
