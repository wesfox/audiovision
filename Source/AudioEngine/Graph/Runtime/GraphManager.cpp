#include "GraphManager.h"

#include "../Model/GraphNode.h"
#include "Core/Edit/Edit.h"
#include "Core/Track/Send.h"

GraphManager::GraphManager(const std::weak_ptr<Edit>& edit,
                           const std::shared_ptr<AudioProcessorGraph>& graph) :
        edit(edit),
        graph(graph)
{
    graph->clear();
    graph->setPlayConfigDetails(1,
        1,
        48000,
        512);
}

void GraphManager::createGraph()
{
    const auto editPtr = edit.lock();
    if (!editPtr) {
        return;
    }
    graphDescription = graphBuilder.buildDescription(*editPtr);
    graphNodes = graphDescription.nodes;
}

GraphNode* GraphManager::getNodeById(String id) {
    for (const auto& node : graphNodes) {
        if (node->getTrackId() == id) {
            return node.get();
        }
    }
    return nullptr;
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

void GraphManager::createFinalGraph(const std::shared_ptr<Transport>& transport) {
    for (auto& node : graphNodes) {
        std::unique_ptr<GraphModule> graphModule = std::make_unique<GraphModule>(node.get(), graph, edit, transport);
        // graphModule.get()->virtualTrack =
        graphModules.emplace_back(std::move(graphModule));
    }
    for (const auto& connection : graphDescription.connections) {
        auto inputModule = getGraphModuleById(connection.inputId);
        auto outputModule = getGraphModuleById(connection.outputId);
        buildConnection(inputModule, outputModule, outputModule->virtualGraphNode->format);
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

void GraphManager::buildConnection(
    const GraphModule* inputModule,
    const GraphModule* outputModule,
    ChannelsFormat format) const
{
    if (inputModule == nullptr || outputModule == nullptr) {
        return;
    }

    for (auto i=0; i<ChannelCount(format); i++) {
        graph->addConnection({
            { inputModule->outputNode->nodeID, i },
            { outputModule->inputNode->nodeID, i }
        });
    }
}
