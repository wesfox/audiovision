#pragma once

#include <JuceHeader.h>

#include "Utils/Format.h"

enum class GraphNodeType {
    AudioTrackGraphNode,
    AuxTrackGraphNode
};

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

class GraphNode {
public:
    GraphNode(String id, String name, GraphNodeType graphNodeType, ChannelsFormat format);

    ~GraphNode() = default;

    /// This function returns a new AudioTrack
    static std::shared_ptr<GraphNode> create(String id, bool isAudioTrack, ChannelsFormat format, const String& name = "")
    {
        return std::make_shared<GraphNode>(
            id,
            name,
            isAudioTrack ? GraphNodeType::AudioTrackGraphNode : GraphNodeType::AuxTrackGraphNode,
            format
        );
    }

    String getTrackId() const { return trackId; };
    String getId() const { return id; };
    String getName() const { return name; };
    [[nodiscard]] ChannelsFormat getFormat() const {return format;};

    void tagIsNotGraphStart(){ isGraphStart = false; };

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

    String toString() {
        return "Node : " + name + " , Track Id : " + id + ", Node Type : " + graphNodeTypeToString(nodeType)+ ", Format : " + ChannelsFormatName(format);
    }

    [[nodiscard]] GraphNodeType getType() const {return nodeType;}

    GraphNodeType nodeType;
    GraphNode* output;
    std::vector<GraphNode*> sends;
    std::vector<void*> inserts;
    bool isGraphStart = true;
    ChannelsFormat format;
private:
    String name;
    String id;
    String trackId;
};
