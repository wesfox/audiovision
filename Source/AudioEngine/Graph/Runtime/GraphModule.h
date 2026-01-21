#pragma once

#include <JuceHeader.h>

#include "../Model/GraphNode.h"
#include "Core/Edit/Edit.h"
#include "Core/Track/AudioTrack.h"
#include "Utils/Format.h"

class Plugin;
class PluginInstanceFactory;
class PluginInstanceStore;

class GraphModule{
public:
    explicit GraphModule(GraphNode * graphNode,
                         const std::weak_ptr<AudioProcessorGraph>& graph,
                         const std::weak_ptr<Edit>& edit,
                         const std::weak_ptr<Transport>& transport,
                         PluginInstanceFactory* pluginFactory,
                         PluginInstanceStore* pluginInstanceStore);

    void buildInternalConnexion(const AudioProcessorGraph::Node *nodeInput, const AudioProcessorGraph::Node *nodeOutput,
                        ChannelsFormat format) const;

    [[nodiscard]] std::weak_ptr<AudioTrack> getAudioTrackById(const String &trackId) const;
    std::shared_ptr<Track> getTrackById(const String& trackId) const;

    juce::AudioProcessorGraph::Node::Ptr inputNode;
    juce::AudioProcessorGraph::Node::Ptr outputNode;

    GraphNode* virtualGraphNode;
    std::weak_ptr<Edit> edit;

    std::weak_ptr<Transport> transport;
private:
    AudioProcessorGraph* getGraphRef() const;
    std::vector<juce::AudioProcessorGraph::Node::Ptr> createPluginNodes(
        const std::vector<std::shared_ptr<Plugin>>& plugins,
        double sampleRate,
        int blockSize);
    void connectPluginChain(const AudioProcessorGraph::Node* inputNode,
                            const AudioProcessorGraph::Node* outputNode,
                            const std::vector<juce::AudioProcessorGraph::Node::Ptr>& plugins,
                            ChannelsFormat format) const;
    void connectPluginsToOutput(const AudioProcessorGraph::Node* outputNode,
                                const std::vector<juce::AudioProcessorGraph::Node::Ptr>& plugins,
                                ChannelsFormat format) const;

    std::weak_ptr<AudioProcessorGraph> graph;
    PluginInstanceFactory* pluginFactory;
    PluginInstanceStore* pluginInstanceStore;
    std::vector<std::unique_ptr<AudioProcessor>> audioProcessors;
    std::vector<juce::AudioProcessorGraph::Node::Ptr> pluginNodes;
};
