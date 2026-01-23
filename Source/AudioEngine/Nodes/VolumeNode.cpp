#include "VolumeNode.h"

#include "AudioEngine/Graph/Model/GraphNode.h"

VolumeNode::VolumeNode(const std::weak_ptr<Transport>& transport,
                       const GraphNode* graphNode,
                       const std::map<ParameterKey, std::atomic<float>*>& parameters)
    : transport(transport),
      graphNode(graphNode)
{
    bindParameters(parameters);
    setPlayConfigDetails(2, 2, 48000.0, 512);
}

const std::vector<ParameterKey>& VolumeNode::requiredParameters()
{
    static const std::vector<ParameterKey> keys = {
        ParameterKey::Volume,
        ParameterKey::StereoPan
    };
    return keys;
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
    const auto* param = getParameter(ParameterKey::Volume);
    const float gain = param ? param->load() : 1.0f;
    const auto* panParam = getParameter(ParameterKey::StereoPan);
    const float pan = panParam ? panParam->load() : 1.0f;
    // juce::Logger::writeToLog(graphNode->getName() + " - " + String(gain));
    // juce::Logger::writeToLog(graphNode->getName() + " - " + String(pan));
    buffer.applyGain(gain);
}
