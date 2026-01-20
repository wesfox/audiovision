#include "GraphModule.h"

#include "AudioEngine/Nodes/AudioTrackNode.h"
#include "AudioEngine/Nodes/VolumeNode.h"

// ------------------------ MainComponent Implementation ------------------------

GraphModule::GraphModule(
    GraphNode * graphNode,
    const std::weak_ptr<AudioProcessorGraph>& graph,
    const std::weak_ptr<Edit>& edit,
    const Transport * transport
    ):
        virtualGraphNode(graphNode),
        edit(edit),
        transport(transport),
        graph(graph)
    {
    const auto graphRef = graph.lock().get();
    if (graphRef == nullptr) {
        throw std::runtime_error("GraphNode is nullptr");
    }

    if (graphNode->getType() == GraphNodeType::AudioTrackGraphNode) {
        auto volumeNode = std::make_unique<VolumeNode>(transport);
        auto graphVolumeNode = graphRef->addNode(std::move(volumeNode));
        outputNode = graphVolumeNode;

        auto track = getAudioTrackById(graphNode->getTrackId());
        auto audioTrackNode = std::make_unique<AudioTrackNode>(track, transport);
        auto graphAudioTrackNode = graphRef->addNode(std::move(audioTrackNode));
        inputNode = graphAudioTrackNode;

        buildInternalConnexion(inputNode.get(), outputNode.get(), graphNode->getFormat());
    }
    else if (graphNode->getType() == GraphNodeType::AuxTrackGraphNode) {
        auto volumeNode = std::make_unique<VolumeNode>(transport);
        auto graphVolumeNode = graphRef->addNode(std::move(volumeNode));
        outputNode = graphVolumeNode;

        inputNode = outputNode;
    }
}

void GraphModule::buildInternalConnexion(const AudioProcessorGraph::Node* nodeInput, const AudioProcessorGraph::Node* nodeOutput, const ChannelsFormat format) const {
    const auto graphRef = graph.lock().get();
    if (graphRef == nullptr) {
        throw std::runtime_error("GraphNode is nullptr");
    }
    for (auto i=0; i<ChannelCount(format); i++) {
        graphRef->addConnection({
            { nodeInput->nodeID, i },
            { nodeOutput->nodeID, i }
        });
    }
}


std::weak_ptr<AudioTrack> GraphModule::getAudioTrackById(const String& trackId) const {
    const auto editPtr = edit.lock();
    if (!editPtr)
        return {};

    for (const auto& t : editPtr->getTracks())
    {
        if (t->getId() == trackId && t->getTrackType() == TrackType::Audio)
            return std::dynamic_pointer_cast<AudioTrack>(t);
    }

    return {};
}

