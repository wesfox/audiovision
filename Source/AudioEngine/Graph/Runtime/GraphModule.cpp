#include "GraphModule.h"

#include "AudioEngine/Nodes/AudioTrackNode.h"
#include "AudioEngine/Nodes/VolumeNode.h"

// ------------------------ MainComponent Implementation ------------------------

GraphModule::GraphModule(
    GraphNode * graphNode,
    const std::weak_ptr<AudioProcessorGraph>& graph,
    const std::weak_ptr<Edit>& edit,
    const std::weak_ptr<Transport>& transport
    ):
        virtualGraphNode(graphNode),
        edit(edit),
        transport(transport),
        graph(graph)
    {
    auto* graphRef = getGraphRef();

    if (graphNode->getType() == GraphNodeType::AudioTrackGraphNode) {
        auto track = getAudioTrackById(graphNode->getTrackId());
        auto audioTrackNode = std::make_unique<AudioTrackNode>(track, transport, graphNode);
        auto graphAudioTrackNode = graphRef->addNode(std::move(audioTrackNode));
        inputNode = graphAudioTrackNode;

        auto volumeNode = std::make_unique<VolumeNode>(transport, graphNode);
        auto graphVolumeNode = graphRef->addNode(std::move(volumeNode));
        outputNode = graphVolumeNode;

        buildInternalConnexion(graphAudioTrackNode.get(), graphVolumeNode.get(), graphNode->getFormat());
    }
    else if (graphNode->getType() == GraphNodeType::AuxTrackGraphNode) {
        auto volumeNode = std::make_unique<VolumeNode>(transport, graphNode);
        auto graphVolumeNode = graphRef->addNode(std::move(volumeNode));
        outputNode = graphVolumeNode;

        inputNode = outputNode;
    }
}

void GraphModule::buildInternalConnexion(const AudioProcessorGraph::Node* nodeInput, const AudioProcessorGraph::Node* nodeOutput, const ChannelsFormat format) const {
    auto* graphRef = getGraphRef();
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

AudioProcessorGraph* GraphModule::getGraphRef() const
{
    auto* graphRef = graph.lock().get();
    if (graphRef == nullptr) {
        throw std::runtime_error("GraphNode is nullptr");
    }
    return graphRef;
}
