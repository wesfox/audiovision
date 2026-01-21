#include "GraphBuilder.h"

#include "Core/Edit/Edit.h"
#include "Core/Track/Send.h"

#include <algorithm>
#include <set>

GraphDescription GraphBuilder::buildDescription(Edit& edit)
{
    GraphDescription description;
    std::set<std::pair<String, String>> seenConnections;
    for (const auto& track : edit.getTracks()) {
        fillGraphNode(track.get(), description.nodes, seenConnections, description.connections);
    }
    return description;
}

GraphNode* GraphBuilder::fillGraphNode(
    Track* track,
    std::vector<std::shared_ptr<GraphNode>>& nodes,
    std::set<std::pair<String, String>>& seenConnections,
    std::vector<GraphConnectionDescription>& connections)
{
    if (track == nullptr) {
        juce::Logger::writeToLog("Got into an empty Track ???");
        return nullptr;
    }

    GraphNode* existingNode = getNodeById(track->getId(), nodes);
    if (existingNode == nullptr) {
        auto newNode = GraphNode::create(
            track->getId(),
            track->isAudioTrack(),
            track->getFormat(),
            track->getName());
        for (auto& send : track->getSends()) {
            GraphNode* existingSendNode = getNodeById(track->getId(), nodes);
            if (existingSendNode == nullptr) {
                auto sendNode = fillGraphNode(
                    send->getDestinationTrack().lock().get(),
                    nodes,
                    seenConnections,
                    connections);
                if (sendNode != nullptr) {
                    sendNode->tagIsNotGraphStart();
                    newNode->sends.push_back(sendNode);
                    addUniqueConnection(newNode.get(), sendNode, seenConnections, connections);
                }
            }
        }
        if (track->getOutput().lock() != nullptr) {
            auto outputNode = fillGraphNode(
                track->getOutput().lock().get(),
                nodes,
                seenConnections,
                connections);
            outputNode->tagIsNotGraphStart();
            newNode->output = outputNode;
            addUniqueConnection(newNode.get(), outputNode, seenConnections, connections);
        }
        nodes.push_back(std::move(newNode));
        return nodes[nodes.size() - 1].get();
    }

    if (track->getOutput().lock() != nullptr) {
        auto outputNode = fillGraphNode(
            track->getOutput().lock().get(),
            nodes,
            seenConnections,
            connections);
        outputNode->tagIsNotGraphStart();
        existingNode->output = outputNode;
        addUniqueConnection(existingNode, outputNode, seenConnections, connections);
    }

    return existingNode;
}

GraphNode* GraphBuilder::getNodeById(
    const String& id,
    std::vector<std::shared_ptr<GraphNode>>& nodes)
{
    for (const auto& node : nodes) {
        if (node->getTrackId() == id) {
            return node.get();
        }
    }
    return nullptr;
}

void GraphBuilder::addUniqueConnection(
    GraphNode* input,
    GraphNode* output,
    std::set<std::pair<String, String>>& seenConnections,
    std::vector<GraphConnectionDescription>& connections)
{
    if (input == nullptr || output == nullptr) {
        return;
    }

    std::pair<String, String> key { input->getId(), output->getId() };
    if (seenConnections.insert(key).second) {
        connections.push_back({ input->getId(), output->getId() });
    }
}
