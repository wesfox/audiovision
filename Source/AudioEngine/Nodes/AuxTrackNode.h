#pragma once

#include "AudioNode.h"
#include "Utils/Transport.h"

class GraphNode;

/// Runtime node for aux/folder track processing.
class AuxTrackNode : public AudioNode {
public:
    /// Create a node bound to transport.
    /// @param transport timeline transport
    /// @param graphNode owning graph node
    AuxTrackNode(const std::weak_ptr<Transport>& transport, const GraphNode* graphNode);

    /// Display name for debugging/graph views.
    const juce::String getName() const override;

    /// Process audio for the aux track node.
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override;

private:
    std::weak_ptr<Transport> transport;
    const GraphNode* graphNode;
};
