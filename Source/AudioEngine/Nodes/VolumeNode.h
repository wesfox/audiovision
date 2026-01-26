#pragma once

#include "AudioNode.h"
#include "Utils/Transport.h"

#include <vector>

class GraphNode;


/// Runtime node applying track-level gain.
class VolumeNode : public AudioNode {
public:
    /// Create a node bound to transport and parameters.
    /// @param transport timeline transport
    /// @param graphNode owning graph node
    /// @param parameters bound parameter map for this node
    VolumeNode(const std::weak_ptr<Transport>& transport,
               const GraphNode* graphNode,
               const std::map<ParameterKey, std::atomic<float>*>& parameters);

    /// Parameters required by this node (used by graph wiring).
    static const std::vector<ParameterKey>& requiredParameters();

    /// Display name for debugging/graph views.
    const juce::String getName() const override;

    /// Apply gain to the buffer.
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override;

private:
    std::weak_ptr<Transport> transport;
    const GraphNode* graphNode;
};
