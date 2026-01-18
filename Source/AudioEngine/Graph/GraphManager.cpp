#include "GraphManager.h"

#include "GraphNode.h"
#include "Core/Edit/Edit.h"
#include "Core/Track/Send.h"

GraphManager::GraphManager()
{
}

GraphNode *GraphManager::fillGraphNode(Track *track) {
    if (track == nullptr) {
        juce::Logger::writeToLog("Got into an empty Track ???");
        return nullptr;
    }
    juce::Logger::writeToLog("Move into Track" + track->getName().toStdString());
    GraphNode *existingNode = getNodeById(track->getId());
    if (existingNode == nullptr) {
        auto newNode = GraphNode::create(track->getId(), track->getName());
        for (auto& send: track->getSends()) {
            GraphNode *existingSendNode = getNodeById(track->getId());
            if (existingSendNode == nullptr) {
                auto sendNode = fillGraphNode(send->getDestinationTrack().lock().get());
                if (sendNode != nullptr) {
                    sendNode->tagIsNotGraphStart();
                    newNode->sends.push_back(sendNode);
                }
            }
        }
        if (track->getOutput().lock() != nullptr) {
            auto outputNode = fillGraphNode(track->getOutput().lock().get());
            outputNode->tagIsNotGraphStart();
            newNode->output = outputNode;
        }
        graphNodes.push_back(std::move(newNode));
        return graphNodes[graphNodes.size() - 1].get();
    }else {
        if (track->getOutput().lock() != nullptr) {
            auto outputNode = fillGraphNode(track->getOutput().lock().get());
            outputNode->tagIsNotGraphStart();
            existingNode->output = outputNode;
        }
        return existingNode;
    }
}

void GraphManager::createGraph(Edit& edit)
{
    for (const auto& track: edit.getTracks()) {
        fillGraphNode(track.get());
    }
}

GraphNode* GraphManager::getNodeById(String id) {
    auto foundNode = std::find_if(graphNodes.begin(), graphNodes.end(),
    [id](const auto& node) {
        return node->getTrackId() == id;
    });
    if (foundNode != graphNodes.end()) {
        return foundNode->get();
    }
    else {
        return nullptr;
    }
}
