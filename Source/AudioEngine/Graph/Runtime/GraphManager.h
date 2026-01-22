#pragma once

#include <JuceHeader.h>
#include <set>

#include "../Builder/GraphBuilder.h"
#include "GraphModule.h"
#include "Core/Track/AudioTrack.h"
#include "AudioEngine/Plugin/PluginChainBuilder.h"
#include "AudioEngine/Plugin/PluginInstanceFactory.h"
#include "AudioEngine/Plugin/PluginInstanceStore.h"
#include "AudioEngine/Parameters/ValueTreeManager.h"

class Track;
class Edit;
class GraphNode;
class RecordSession;

class GraphManager {
public:
    GraphManager(const std::weak_ptr<Edit>& edit,
                 const std::shared_ptr<AudioProcessorGraph>& graph,
                 RecordSession* recordSession);

    void createGraphFromEdit();

    ~GraphManager() = default;

    const std::weak_ptr<Edit> edit;

    std::vector<std::shared_ptr<GraphNode>> graphNodes;
    std::vector<std::unique_ptr<GraphModule>> graphModules;

    GraphNode* getNodeById(String id);
    GraphModule *getGraphModuleById(String id);
    GraphModule *getGraphModuleByTrackId(String id);

    void createFinalGraph(const std::shared_ptr<Transport>& transport);

    void attachAudioOutput(std::weak_ptr<Track> track);

    void prepareToPlay(double sampleRate, int blockSize) const;

    std::shared_ptr<AudioProcessorGraph> graph;

    const GraphDescription& getGraphDescription() const { return graphDescription; }

    void setProcessingFormat(double sampleRate, int blockSize);
    void shutdown();
    juce::AudioProcessorGraph::Node::Ptr findPluginNode(const String& trackId,
                                                        const String& pluginName) const;

    ValueTreeManager& getValueTreeManager() const { return *valueTreeManager; }

private:
    void buildConnection(const GraphModule* inputModule,
                         const GraphModule* outputModule,
                         ChannelsFormat format) const;

    juce::AudioProcessorGraph::Node::Ptr audioOutputNode;
    GraphBuilder graphBuilder;
    GraphDescription graphDescription;
    std::unique_ptr<PluginInstanceFactory> pluginFactory;
    PluginInstanceStore pluginInstanceStore;
    std::unique_ptr<PluginChainBuilder> pluginChainBuilder;
    RecordSession* recordSession = nullptr;
    std::unique_ptr<ValueTreeManager> valueTreeManager;
};
