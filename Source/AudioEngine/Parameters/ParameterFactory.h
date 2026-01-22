#pragma once

#include <JuceHeader.h>

#include <vector>

class GraphNode;

struct ParameterDescriptor {
    juce::String name;
    float minValue = 0.0f;
    float maxValue = 1.0f;
    float defaultValue = 0.0f;
    juce::String label;
};

class ParameterFactory {
public:
    std::vector<ParameterDescriptor> collectForNode(const GraphNode& graphNode) const;
};
