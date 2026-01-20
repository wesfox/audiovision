#include "VolumeNode.h"

#include "AudioEngine/Graph/Model/GraphNode.h"

VolumeNode::VolumeNode(const std::weak_ptr<Transport>& transport, const GraphNode* graphNode)
    : transport(transport),
      graphNode(graphNode)
{
    setPlayConfigDetails(2, 2, 48000.0, 512);
}

const juce::String VolumeNode::getName() const
{
    if (graphNode != nullptr) {
        return graphNode->getName() + ":VolumeNode";
    }
    return "NoName:VolumeNode";
}

void VolumeNode::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
    buffer.applyGain(1.0f); // Apply unity gain
}
