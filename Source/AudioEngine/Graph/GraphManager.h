#pragma once

#include <JuceHeader.h>
#include <set>

#include "VirtualConnections/GraphVirtualConnection.h"
#include "GraphModule.h"
#include "VirtualConnections/GraphConnectionManager.h"

class Track;
class Edit;

class GraphNode;

class GraphManager {
public:
    GraphManager(std::shared_ptr<AudioProcessorGraph> graph = nullptr);

    GraphNode *fillGraphNode(Track *track);

    ~GraphManager() = default;

    // Methods
    void createGraph(Edit &edit);

    std::vector<std::shared_ptr<GraphNode>> graphNodes;
    std::vector<std::unique_ptr<GraphModule>> graphModules;

    GraphConnectionManager graphConnectionManager;

    GraphNode* getNodeById(String id);

    GraphModule *getGraphModuleById(String id);

    void createFinalGraph();

    void attachAudioOutput(std::weak_ptr<Track> track);

    std::shared_ptr<AudioProcessorGraph> graph;

private:
    juce::AudioProcessorGraph::Node::Ptr audioOutputNode;
};
