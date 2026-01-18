#pragma once

#include <JuceHeader.h>

enum class GraphNodeType {
    AudioTrackGraphNode,
    AuxTrackGraphNode
};

class GraphNode {
public:
    GraphNode(String id, String name);
    ~GraphNode() = default;

    /// This function returns a new AudioTrack
    static std::shared_ptr<GraphNode> create(String id, const String& name = "")
    {
        return std::make_shared<GraphNode>(id, name);
    }

    String getTrackId() {return trackId;};

    void tagIsNotGraphStart(){ isGraphStart = false; };

    static void logGraph(GraphNode * node) {
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

    GraphNodeType nodeType;
    GraphNode* output;
    std::vector<GraphNode*> sends;
    std::vector<void*> inserts;
    bool isGraphStart = true;
private:
    String name;
    String id;
    String trackId;
};
