#include "GraphNode.h"
#include <Utils/Uuid.h>

GraphNode::GraphNode(String id)
    : id(id),
      trackId(""),
      nodeType(GraphNodeType::AudioTrackGraphNode),
      output(nullptr)
{
}
