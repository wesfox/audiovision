#include "GraphModule.h"

#include "AudioEngine/Nodes/AudioTrackNode.h"
#include "AudioEngine/Nodes/VolumeNode.h"
#include "AudioEngine/Plugin/PluginChainBuilder.h"
#include "AudioEngine/Recording/RecordSession.h"
#include "AudioEngine/Parameters/ValueTreeManager.h"

// ------------------------ MainComponent Implementation ------------------------

GraphModule::GraphModule(
    GraphNode * graphNode,
    const std::weak_ptr<AudioProcessorGraph>& graph,
    const std::weak_ptr<Edit>& edit,
    const std::weak_ptr<Transport>& transport,
    PluginChainBuilder* pluginChainBuilder,
    RecordSession* recordSession,
    ValueTreeManager* valueTreeManager
    ):
        virtualGraphNode(graphNode),
        edit(edit),
        transport(transport),
        graph(graph),
        pluginChainBuilder(pluginChainBuilder),
        recordSession(recordSession),
        valueTreeManager(valueTreeManager)
    {
    auto* graphRef = getGraphRef();

    if (graphNode->getType() == GraphNodeType::AudioTrackGraphNode) {
        auto track = getAudioTrackById(graphNode->getTrackId());
        auto audioTrackNode = std::make_unique<AudioTrackNode>(track, transport, graphNode);
        auto* audioTrackNodePtr = audioTrackNode.get();
        auto graphAudioTrackNode = graphRef->addNode(std::move(audioTrackNode));
        inputNode = graphAudioTrackNode;
        if (recordSession != nullptr && graphAudioTrackNode != nullptr) {
            recordSession->registerTrackNode(graphNode->getTrackId(), audioTrackNodePtr);
        }

        auto trackPtr = getTrackById(graphNode->getTrackId());
        auto parameters = valueTreeManager
            ? valueTreeManager->buildParamMap(graphNode->getTrackId(), VolumeNode::requiredParameters())
            : std::map<ParameterKey, std::atomic<float>*>{};
        auto volumeNode = std::make_unique<VolumeNode>(transport, graphNode, trackPtr, parameters);
        auto graphVolumeNode = graphRef->addNode(std::move(volumeNode));
        outputNode = graphVolumeNode;

        if (trackPtr != nullptr) {
            auto transportPtr = transport.lock();
            auto sampleRate = transportPtr ? transportPtr->getSampleRate() : 48000.0;
            auto blockSize = transportPtr ? transportPtr->getCurrentBlockSize() : 512;
            if (pluginChainBuilder != nullptr) {
                auto plugins = pluginChainBuilder->createPluginNodes(
                    trackPtr->getPlugins(),
                    *graphRef,
                    graphNode->getTrackId(),
                    sampleRate,
                    blockSize);
                pluginChainBuilder->connectChain(
                    *graphRef,
                    graphAudioTrackNode.get(),
                    graphVolumeNode.get(),
                    plugins,
                    graphNode->getFormat());
            } else {
                connectNodes(*graphRef, graphAudioTrackNode.get(), graphVolumeNode.get(), graphNode->getFormat());
            }
        } else {
            connectNodes(*graphRef, graphAudioTrackNode.get(), graphVolumeNode.get(), graphNode->getFormat());
        }
    }
    else if (graphNode->getType() == GraphNodeType::AuxTrackGraphNode) {
        auto trackPtr = getTrackById(graphNode->getTrackId());
        auto parameters = valueTreeManager
            ? valueTreeManager->buildParamMap(graphNode->getTrackId(), VolumeNode::requiredParameters())
            : std::map<ParameterKey, std::atomic<float>*>{};
        auto volumeNode = std::make_unique<VolumeNode>(transport, graphNode, trackPtr, parameters);
        auto graphVolumeNode = graphRef->addNode(std::move(volumeNode));
        outputNode = graphVolumeNode;

        if (trackPtr != nullptr) {
            auto transportPtr = transport.lock();
            auto sampleRate = transportPtr ? transportPtr->getSampleRate() : 48000.0;
            auto blockSize = transportPtr ? transportPtr->getCurrentBlockSize() : 512;
            if (pluginChainBuilder != nullptr) {
                auto plugins = pluginChainBuilder->createPluginNodes(
                    trackPtr->getPlugins(),
                    *graphRef,
                    graphNode->getTrackId(),
                    sampleRate,
                    blockSize);
                if (!plugins.empty()) {
                    inputNode = plugins.front();
                    pluginChainBuilder->connectPluginsToOutput(
                        *graphRef,
                        graphVolumeNode.get(),
                        plugins,
                        graphNode->getFormat());
                } else {
                    inputNode = outputNode;
                }
            } else {
                inputNode = outputNode;
            }
        } else {
            inputNode = outputNode;
        }
    }
}

std::weak_ptr<AudioTrack> GraphModule::getAudioTrackById(const String& trackId) const {
    const auto editPtr = edit.lock();
    if (!editPtr)
        return {};

    for (const auto& t : editPtr->getTracks())
    {
        if (t->getId() == trackId && t->isAudioTrack())
            return std::dynamic_pointer_cast<AudioTrack>(t);
    }

    return {};
}

std::shared_ptr<Track> GraphModule::getTrackById(const String& trackId) const
{
    const auto editPtr = edit.lock();
    if (!editPtr) {
        return {};
    }

    for (const auto& t : editPtr->getTracks())
    {
        if (t->getId() == trackId) {
            return t;
        }
    }

    return {};
}

AudioProcessorGraph* GraphModule::getGraphRef() const
{
    auto* graphRef = graph.lock().get();
    if (graphRef == nullptr) {
        throw std::runtime_error("GraphNode is nullptr");
    }
    return graphRef;
}

void GraphModule::connectNodes(AudioProcessorGraph& graph,
                               const AudioProcessorGraph::Node* nodeInput,
                               const AudioProcessorGraph::Node* nodeOutput,
                               ChannelsFormat format)
{
    for (auto i = 0; i < ChannelCount(format); i++) {
        graph.addConnection({
            { nodeInput->nodeID, i },
            { nodeOutput->nodeID, i }
        });
    }
}
