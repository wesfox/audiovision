#include "GraphMermaidExporter.h"

#include "Utils/Format.h"

#include <unordered_map>

juce::String GraphMermaidExporter::exportMermaid(const GraphDescription& description)
{
    juce::String output;
    output << "graph TD\n";

    std::unordered_map<String, String> nodeIdMap;
    std::unordered_map<String, ChannelsFormat> nodeFormatMap;
    nodeIdMap.reserve(static_cast<size_t>(description.nodes.size()));
    nodeFormatMap.reserve(static_cast<size_t>(description.nodes.size()));

    int index = 0;
    for (const auto& node : description.nodes) {
        auto mermaidId = "N" + String(index++);
        nodeIdMap.emplace(node->getId(), mermaidId);
        nodeFormatMap.emplace(node->getId(), node->getFormat());

        juce::String label = node->getName();
        if (label.isEmpty()) {
            label = node->getId();
        }
        output << "  " << mermaidId
               << "[\"" << label.replace("\"", "\\\"")
               << "\"]\n";
    }

    std::unordered_map<String, int> edgeChannelCounts;
    edgeChannelCounts.reserve(static_cast<size_t>(description.connections.size()));

    for (const auto& connection : description.connections) {
        auto inputIt = nodeFormatMap.find(connection.inputId);
        auto outputIt = nodeFormatMap.find(connection.outputId);
        auto formatIt = outputIt != nodeFormatMap.end() ? outputIt : inputIt;
        int channelCount = 1;
        if (formatIt != nodeFormatMap.end()) {
            channelCount = ChannelCount(formatIt->second);
        }
        auto key = connection.inputId + "->" + connection.outputId;
        edgeChannelCounts[key] = channelCount;
    }

    for (const auto& connection : description.connections) {
        auto inputIt = nodeIdMap.find(connection.inputId);
        auto outputIt = nodeIdMap.find(connection.outputId);
        if (inputIt == nodeIdMap.end() || outputIt == nodeIdMap.end()) {
            continue;
        }
        auto key = connection.inputId + "->" + connection.outputId;
        auto countIt = edgeChannelCounts.find(key);
        auto count = countIt != edgeChannelCounts.end() ? countIt->second : 1;
        output << "  " << inputIt->second << " -- \""
               << count << "\" --> " << outputIt->second << "\n";
    }

    return output;
}
