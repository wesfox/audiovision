#pragma once

#include <JuceHeader.h>

enum class GraphNodeType {
    AudioTrackGraphNode,
    AuxTrackGraphNode
};

class GraphNode {
public:
    GraphNode(String id);
    ~GraphNode() = default;

    /// This function returns a new AudioTrack
    static std::shared_ptr<GraphNode> create(String id)
    {
        return std::make_shared<GraphNode>(id);
    }

    String getId() {return id;};

    GraphNodeType nodeType;
    GraphNode* output;
    std::vector<GraphNode*> sends;
    std::vector<void*> inserts;
private:
    String id;
    String trackId;
};
