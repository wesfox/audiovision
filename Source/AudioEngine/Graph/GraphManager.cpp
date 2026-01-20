#include "GraphManager.h"

#include "GraphNode.h"
#include "AudioEngine/AudioEngine.h"
#include "Core/Edit/Edit.h"
#include "Core/Track/Send.h"

GraphManager::GraphManager(const std::weak_ptr<Edit>& edit, const AudioEngine* engine) :
        edit(edit)
{
    graph = engine->getAudioGraph();
    graph->clear();
    graph->setPlayConfigDetails(1,
        1,
        48000,
        512);
    graphConnectionManager = std::make_unique<GraphConnectionManager>(graphNodes, graph);
}

GraphNode *GraphManager::fillGraphNode(Track *track) {
    if (track == nullptr) {
        juce::Logger::writeToLog("Got into an empty Track ???");
        return nullptr;
    }
    juce::Logger::writeToLog("Move into Track" + track->getName().toStdString());
    GraphNode *existingNode = getNodeById(track->getId());
    if (existingNode == nullptr) {
        auto newNode = GraphNode::create(track->getId(), track->isAudioTrack(), track->getFormat(), track->getName());
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

void GraphManager::createGraph()
{
    for (const auto& track: edit.lock()->getTracks()) {
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

GraphModule* GraphManager::getGraphModuleById(String id) {
    for (auto& module: graphModules) {
        if (module->virtualGraphNode->getId() == id) {
            return module.get();
        }
    }
    return nullptr;
}

GraphModule * GraphManager::getGraphModuleByTrackId(String trackId) {
    for (auto& module: graphModules) {
        if (module->virtualGraphNode->getTrackId() == trackId) {
            return module.get();
        }
    }
    return nullptr;
}

void GraphManager::createFinalGraph(Transport * transport) {
    graphConnectionManager->buildGraphVirtualConnections();
    for (auto& node : graphNodes) {
        std::unique_ptr<GraphModule> graphModule = std::make_unique<GraphModule>(node.get(), graph, edit, transport);
        // graphModule.get()->virtualTrack =
        graphModules.emplace_back(std::move(graphModule));
    }
    for (auto& connection: graphConnectionManager->graphVirtualConnections) {
        auto inputModule = getGraphModuleById(connection->inputId);
        auto outputModule = getGraphModuleById(connection->outputId);
        graphConnectionManager->buildConnection(inputModule, outputModule, outputModule->virtualGraphNode->format);
    }
}

void GraphManager::attachAudioOutput(std::weak_ptr<Track> track) {
    auto newAudioOutputNode = graph->addNode(
        std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
            juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode)
    );
    auto outputModule = getGraphModuleByTrackId(track.lock()->getId());
    for (auto i=0; i<ChannelCount(track.lock()->getFormat()); i++) {
        graph->addConnection({
            { outputModule->outputNode->nodeID, i },
            { newAudioOutputNode->nodeID, i }
        });
    }
    audioOutputNode = newAudioOutputNode;
}

void GraphManager::prepareToPlay() const {
    graph->prepareToPlay(1, 512);
}
