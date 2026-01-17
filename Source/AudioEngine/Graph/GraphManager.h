#pragma once

#include <JuceHeader.h>

class Track;
class Edit;

class GraphNode;

class GraphManager {
public:
    GraphManager();

    GraphNode *fillGraphNode(Track *track);

    ~GraphManager() = default;

    // Methods
    void createGraph(Edit &edit);

    std::vector<std::shared_ptr<GraphNode>> graphNodes;

    GraphNode* getNodeById(String id);

private:
};
