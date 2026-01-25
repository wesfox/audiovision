#pragma once

#include "AudioNode.h"
#include "Utils/Transport.h"

#include <vector>

class GraphNode;


class VolumeNode : public AudioNode {
public:
    VolumeNode(const std::weak_ptr<Transport>& transport,
               const GraphNode* graphNode,
               const std::map<ParameterKey, std::atomic<float>*>& parameters);
    static const std::vector<ParameterKey>& requiredParameters();

    const juce::String getName() const override;

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override;

private:
    std::weak_ptr<Transport> transport;
    const GraphNode* graphNode;
};
