#pragma once

#include <JuceHeader.h>

#include "Utils/Format.h"

/// Graph node type for routing semantics.
enum class GraphNodeType {
    AudioTrackGraphNode,
    AuxTrackGraphNode
};

/// Convert node type to a debug string.
static const char* graphNodeTypeToString(const GraphNodeType type)
{
    switch (type) {
        case GraphNodeType::AudioTrackGraphNode:
            return "AudioTrackGraphNode";
        case GraphNodeType::AuxTrackGraphNode:
            return "AuxTrackGraphNode";
        default:
            return "UnknownGraphNodeType";
    }
}

/// Graph node descriptor used by graph building and runtime wiring.
class GraphNode {
public:
    GraphNode(String id, String name, GraphNodeType graphNodeType, ChannelsFormat format);

    ~GraphNode() = default;

    /// Create a graph node with a generated internal id.
    /// @param id track id
    /// @param isAudioTrack true for audio tracks, false for aux tracks
    /// @param format channel format for this node
    /// @param name display name (optional)
    static std::shared_ptr<GraphNode> create(String id, bool isAudioTrack, ChannelsFormat format, const String& name = "")
    {
        return std::make_shared<GraphNode>(
            id,
            name,
            isAudioTrack ? GraphNodeType::AudioTrackGraphNode : GraphNodeType::AuxTrackGraphNode,
            format
        );
    }

    /// Track id this node represents.
    String getTrackId() const { return trackId; };

    /// Internal graph node id (unique per graph build).
    String getId() const { return id; };

    /// Display name for debugging/graph views.
    String getName() const { return name; };

    /// Channel format for routing and connections.
    [[nodiscard]] ChannelsFormat getFormat() const {return format;};

    /// Mark this node as not being a graph start.
    void tagIsNotGraphStart(){ isGraphStart = false; };

    /// Log this node and its send/output connections (debug helper).
    static void logGraph(const GraphNode * node) {
        juce::Logger::writeToLog("node : " + node->name);
        String sends = "";
        for (auto send : node->sends) {
            sends += send->name + ", ";
        }
        juce::Logger::writeToLog("Send TO : " + sends);
        if (node->output != nullptr)
            juce::Logger::writeToLog("Output : " + node->output->name);
        juce::Logger::writeToLog("==================");
        for (auto send : node->sends) {
            logGraph(send);
        }
        if (node->output != nullptr)
            logGraph(node->output);
    }

    /// Debug string representation.
    String toString() {
        return "Node : " + name + " , Track Id : " + id + ", Node Type : " + graphNodeTypeToString(nodeType)+ ", Format : " + ChannelsFormatName(format);
    }

    /// Node type accessor.
    [[nodiscard]] GraphNodeType getType() const {return nodeType;}

    /// Node type used for graph wiring decisions.
    GraphNodeType nodeType;
    /// Output node in the routing graph (may be null).
    GraphNode* output;
    /// Send targets from this node.
    std::vector<GraphNode*> sends;
    /// Insert slots (runtime-owned pointers).
    std::vector<void*> inserts;
    /// True when this node has no upstream parent.
    bool isGraphStart = true;
    /// Channel format for this node.
    ChannelsFormat format;
private:
    String name;
    String id;
    String trackId;
};
