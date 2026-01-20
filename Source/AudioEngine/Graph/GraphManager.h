#pragma once

#include <JuceHeader.h>
#include <set>

#include "GraphModule.h"
#include "Core/Track/AudioTrack.h"
#include "VirtualConnections/GraphConnectionManager.h"

class Track;
class Edit;
class AudioEngine;

class GraphNode;

class GraphManager {
public:
    GraphManager(const std::weak_ptr<Edit> &edit, const AudioEngine *engine);

    GraphNode *fillGraphNode(Track *track);

    ~GraphManager() = default;

    // Methods
    void createGraph();

    const std::weak_ptr<Edit> edit;

    std::vector<std::shared_ptr<GraphNode>> graphNodes;
    std::vector<std::unique_ptr<GraphModule>> graphModules;

     std::unique_ptr<GraphConnectionManager> graphConnectionManager;

    GraphNode* getNodeById(String id);
    GraphModule *getGraphModuleById(String id);
    GraphModule *getGraphModuleByTrackId(String id);

    void createFinalGraph(const std::shared_ptr<Transport>& transport);

    void attachAudioOutput(std::weak_ptr<Track> track);

    void prepareToPlay() const;

    std::shared_ptr<AudioProcessorGraph> graph;

private:
    juce::AudioProcessorGraph::Node::Ptr audioOutputNode;
};
