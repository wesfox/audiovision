#pragma once

#include <JuceHeader.h>
#include <set>

#include "../Model/GraphNode.h"

class Edit;
class Track;

/// Directed connection between two graph node ids.
struct GraphConnectionDescription {
    String inputId;
    String outputId;
};

/// Snapshot of graph nodes and their connections.
struct GraphDescription {
    std::vector<std::shared_ptr<GraphNode>> nodes;
    std::vector<GraphConnectionDescription> connections;
};

/// Build a graph description from an Edit (used when creating runtime graphs).
class GraphBuilder {
public:
    /// Build nodes and connections from the edit.
    /// @param edit edit containing tracks and routing
    GraphDescription buildDescription(Edit& edit);

private:
    /// Create or reuse a graph node for a track and append connections.
    /// @param track source track
    /// @param nodes accumulator of graph nodes
    /// @param seenConnections deduplication set
    /// @param connections accumulator of graph connections
    GraphNode* fillGraphNode(
        Track* track,
        std::vector<std::shared_ptr<GraphNode>>& nodes,
        std::set<std::pair<String, String>>& seenConnections,
        std::vector<GraphConnectionDescription>& connections);

    /// Find a node by track id within the current node list.
    /// @param id track id to search
    /// @param nodes current node list
    GraphNode* getNodeById(
        const String& id,
        std::vector<std::shared_ptr<GraphNode>>& nodes);

    /// Add a connection if it has not been added already.
    /// @param input upstream node
    /// @param output downstream node
    /// @param seenConnections deduplication set
    /// @param connections accumulator of graph connections
    void addUniqueConnection(
        GraphNode* input,
        GraphNode* output,
        std::set<std::pair<String, String>>& seenConnections,
        std::vector<GraphConnectionDescription>& connections);
};
