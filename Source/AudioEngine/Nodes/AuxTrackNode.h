#pragma once

#include "AudioNode.h"
#include "Utils/Transport.h"

class GraphNode;

class AuxTrackNode : public AudioNode {
public:
    AuxTrackNode(const std::weak_ptr<Transport>& transport, const GraphNode* graphNode);

    const juce::String getName() const override;

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override;

private:
    std::weak_ptr<Transport> transport;
    const GraphNode* graphNode;
};
