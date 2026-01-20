#include "AudioGraphMermaidExporter.h"

#include <unordered_map>

namespace {
juce::String getNodeLabel(const juce::AudioProcessorGraph::Node* node)
{
    if (node == nullptr || node->getProcessor() == nullptr) {
        return "Unknown";
    }

    if (auto* io = dynamic_cast<juce::AudioProcessorGraph::AudioGraphIOProcessor*>(node->getProcessor())) {
        switch (io->getType()) {
            case juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode:
                return "Audio Input";
            case juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode:
                return "Audio Output";
            case juce::AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode:
                return "MIDI Input";
            case juce::AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode:
                return "MIDI Output";
            default:
                return "Audio IO";
        }
    }

    return node->getProcessor()->getName();
}
}

juce::String AudioGraphMermaidExporter::exportMermaid(const juce::AudioProcessorGraph& graph)
{

    const auto& nodes = graph.getNodes();
    const auto connections = graph.getConnections();

    juce::String output;
    output << "%% Number of nodes : " << String(nodes.size()) << ", Number of connections : " << String(connections.size()) << "\n";
    output << "graph TD\n";

    // Add nodes
    std::unordered_map<uint32_t, juce::String> nodeIdMap;
    nodeIdMap.reserve(static_cast<size_t>(nodes.size()));

    for (const auto* node : nodes) {
        if (node == nullptr) {
            continue;
        }
        auto nodeUid = node->nodeID.uid;
        auto mermaidId = "N" + juce::String(static_cast<int>(nodeUid));
        nodeIdMap.emplace(nodeUid, mermaidId);

        auto label = getNodeLabel(node);
        output << "  " << mermaidId
               << "[\"" << label.replace("\"", "\\\"")
               << "\"]\n";
    }

    // add Connections
    std::unordered_map<uint64_t, int> edgeChannelCounts;
    edgeChannelCounts.reserve(static_cast<size_t>(connections.size()));

    for (const auto& connection : connections) {
        uint64_t key = (static_cast<uint64_t>(connection.source.nodeID.uid) << 32)
                       | connection.destination.nodeID.uid;
        edgeChannelCounts[key] += 1;
    }

    for (const auto& entry : edgeChannelCounts) {
        auto sourceUid = static_cast<uint32_t>(entry.first >> 32);
        auto destUid = static_cast<uint32_t>(entry.first & 0xFFFFFFFFu);
        auto sourceIt = nodeIdMap.find(sourceUid);
        auto destIt = nodeIdMap.find(destUid);
        if (sourceIt == nodeIdMap.end() || destIt == nodeIdMap.end()) {
            continue;
        }
        auto count = entry.second;
        output << "  " << sourceIt->second << " -- \""
               << count << "\" --> " << destIt->second << "\n";
    }

    return output;
}
