#include "GraphNode.h"

#include <utility>

#include "Utils/Uuid.h"

GraphNode::GraphNode(String id, String name, const GraphNodeType graphNodeType, const ChannelsFormat format):
        nodeType(graphNodeType),
        output(nullptr),
        name(std::move(name)),
        id(uuid::generate_uuid_v4()),
        trackId(std::move(id)),
        format(format)
{
}
