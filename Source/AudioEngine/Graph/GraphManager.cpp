#include "GraphManager.h"

#include "GraphNode.h"
#include "Core/Edit/Edit.h"
#include "Core/Track/Send.h"

GraphManager::GraphManager()
{
}

GraphNode *GraphManager::fillGraphNode(Track *track) {
    if (track == nullptr) return nullptr;
    GraphNode *existingNode = getNodeById(track->getId());
    if (existingNode == nullptr) {
        auto newNode = GraphNode::create(track->getId());
        for (auto& send: track->getSends()) {
            GraphNode *existingSendNode = getNodeById(track->getId());
            if (existingSendNode == nullptr) {
                auto sendNode = fillGraphNode(send->getDestinationTrack().lock().get());
                newNode->sends.push_back(sendNode);
                sendNode->output = sendNode;
            }
        }
        if (track->getOutput().lock() != nullptr) {
            auto outputNode = fillGraphNode(track->getOutput().lock().get());
            newNode->output = outputNode;
        }
        graphNodes.push_back(std::move(newNode));
    }else {
        if (track->getOutput().lock() != nullptr) {
            auto outputNode = fillGraphNode(track->getOutput().lock().get());
            existingNode->output = outputNode;
        }
        return existingNode;
    }
}

void GraphManager::createGraph(Edit& edit)
{
    for (auto track: edit.getTracks()) {
        fillGraphNode(track.get());
    }
}

GraphNode* GraphManager::getNodeById(String id) {
    auto foundNode = std::find_if(graphNodes.begin(), graphNodes.end(),
    [id](const auto& node) {
        return node->getId() == id;
    });
    if (foundNode != graphNodes.end()) {
        return foundNode->get();
    }
    else {
        return nullptr;
    }
}
