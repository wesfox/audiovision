#include "AuxTrackNode.h"

#include "AudioEngine/Graph/Model/GraphNode.h"

AuxTrackNode::AuxTrackNode(const std::weak_ptr<Transport>& transport, const GraphNode* graphNode)
    : transport(transport),
      graphNode(graphNode)
{
    setPlayConfigDetails(2, 2, 48000.0, 512);
}

const juce::String AuxTrackNode::getName() const
{
    if (graphNode != nullptr) {
        return graphNode->getName() + ":AuxTrackNode";
    }
    return "NoName:AuxTrackNode";
}

void AuxTrackNode::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    // Return buffer directly without any processing
}
