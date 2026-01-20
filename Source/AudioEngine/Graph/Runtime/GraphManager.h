#pragma once

#include <JuceHeader.h>
#include <set>

#include "../Builder/GraphBuilder.h"
#include "GraphModule.h"
#include "Core/Track/AudioTrack.h"

class Track;
class Edit;
class AudioEngine;

class GraphNode;

class GraphManager {
public:
    GraphManager(const std::weak_ptr<Edit> &edit, const AudioEngine *engine);

    ~GraphManager() = default;

    // Methods
    void createGraph();

    const std::weak_ptr<Edit> edit;

    std::vector<std::shared_ptr<GraphNode>> graphNodes;
    std::vector<std::unique_ptr<GraphModule>> graphModules;

    GraphNode* getNodeById(String id);
    GraphModule *getGraphModuleById(String id);
    GraphModule *getGraphModuleByTrackId(String id);

    void createFinalGraph(const std::shared_ptr<Transport>& transport);

    void attachAudioOutput(std::weak_ptr<Track> track);

    void prepareToPlay() const;

    std::shared_ptr<AudioProcessorGraph> graph;

    const GraphDescription& getGraphDescription() const { return graphDescription; }

private:
    void buildConnection(const GraphModule* inputModule,
                         const GraphModule* outputModule,
                         ChannelsFormat format) const;

    juce::AudioProcessorGraph::Node::Ptr audioOutputNode;
    GraphBuilder graphBuilder;
    GraphDescription graphDescription;
};
