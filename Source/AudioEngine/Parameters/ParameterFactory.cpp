#include "ParameterFactory.h"

#include "AudioEngine/Graph/Model/GraphNode.h"

std::vector<ParameterDescriptor> ParameterFactory::collectForNode(const GraphNode& /*graphNode*/) const
{
    ParameterDescriptor volume;
    volume.name = "volume";
    volume.minValue = 0.0f;
    volume.maxValue = 1.0f;
    volume.defaultValue = 1.0f;
    volume.label = "";

    return { volume };
}
