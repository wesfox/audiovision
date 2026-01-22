#include "VolumeNode.h"

#include "AudioEngine/Graph/Model/GraphNode.h"

VolumeNode::VolumeNode(const std::weak_ptr<Transport>& transport,
                       const GraphNode* graphNode,
                       std::atomic<float>* volumeParam)
    : transport(transport),
      graphNode(graphNode),
      volumeParam(volumeParam)
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
    bool nonFiniteValueAlert=false;
    for (int c=0;c!= buffer.getNumChannels();c++) {
        for (int i=0;i!=buffer.getNumSamples();i++) {
            if (!std::isfinite(buffer.getSample(c,i))) {
                buffer.setSample(c,i,0);
                nonFiniteValueAlert = true;
            }
        }
    }
    // We got here because at least one sample was not a finite value
    jassert (!nonFiniteValueAlert);
    const float volume = volumeParam ? volumeParam->load() : 1.0f;
    juce::Logger::writeToLog(getName() + String(volume));
    buffer.applyGain(volume);
}
