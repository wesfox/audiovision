#pragma once

#include <JuceHeader.h>
#include <set>

#include "../Model/GraphNode.h"
#include "Core/Edit/Scene.h"

class Edit;
class Track;

struct GraphConnectionDescription {
    String inputId;
    String outputId;
};

struct GraphDescription {
    std::vector<std::shared_ptr<GraphNode>> nodes;
    std::vector<GraphConnectionDescription> connections;
};

class GraphBuilder {
public:
    GraphDescription buildDescription(Edit& edit);

    GraphDescription buildDescription(const Scene *scene);

private:
    GraphNode* fillGraphNode(
        Track* track,
        std::vector<std::shared_ptr<GraphNode>>& nodes,
        std::set<std::pair<String, String>>& seenConnections,
        std::vector<GraphConnectionDescription>& connections);
    GraphNode* getNodeById(
        const String& id,
        std::vector<std::shared_ptr<GraphNode>>& nodes);

    void addUniqueConnection(
        GraphNode* input,
        GraphNode* output,
        std::set<std::pair<String, String>>& seenConnections,
        std::vector<GraphConnectionDescription>& connections);
};
