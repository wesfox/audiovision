#include "GraphNode.h"

#include <utility>

GraphNode::GraphNode(String id, String  name)
    : id(""),
      trackId(id),
        name(std::move(name)),
      nodeType(GraphNodeType::AudioTrackGraphNode),
      output(nullptr)
{
}
