#include "GraphModule.h"

#include "AudioEngine/Nodes/AudioTrackNode.h"
#include "AudioEngine/Nodes/VolumeNode.h"
#include "AudioEngine/Plugin/PluginInstanceFactory.h"
#include "AudioEngine/Plugin/PluginInstanceStore.h"
#include "Core/Plugin/Plugin.h"

// ------------------------ MainComponent Implementation ------------------------

GraphModule::GraphModule(
    GraphNode * graphNode,
    const std::weak_ptr<AudioProcessorGraph>& graph,
    const std::weak_ptr<Edit>& edit,
    const std::weak_ptr<Transport>& transport,
    PluginInstanceFactory* pluginFactory,
    PluginInstanceStore* pluginInstanceStore
    ):
        virtualGraphNode(graphNode),
        edit(edit),
        transport(transport),
        graph(graph),
        pluginFactory(pluginFactory),
        pluginInstanceStore(pluginInstanceStore)
    {
    auto* graphRef = getGraphRef();

    if (graphNode->getType() == GraphNodeType::AudioTrackGraphNode) {
        auto track = getAudioTrackById(graphNode->getTrackId());
        auto audioTrackNode = std::make_unique<AudioTrackNode>(track, transport, graphNode);
        auto graphAudioTrackNode = graphRef->addNode(std::move(audioTrackNode));
        inputNode = graphAudioTrackNode;

        auto volumeNode = std::make_unique<VolumeNode>(transport, graphNode);
        auto graphVolumeNode = graphRef->addNode(std::move(volumeNode));
        outputNode = graphVolumeNode;

        auto trackPtr = getTrackById(graphNode->getTrackId());
        if (trackPtr != nullptr) {
            auto transportPtr = transport.lock();
            auto sampleRate = transportPtr ? transportPtr->getSampleRate() : 48000.0;
            auto blockSize = transportPtr ? transportPtr->getCurrentBlockSize() : 512;
            auto plugins = createPluginNodes(trackPtr->getPlugins(), sampleRate, blockSize);
            if (!plugins.empty()) {
                connectPluginChain(graphAudioTrackNode.get(), graphVolumeNode.get(), plugins, graphNode->getFormat());
            } else {
                buildInternalConnexion(graphAudioTrackNode.get(), graphVolumeNode.get(), graphNode->getFormat());
            }
        } else {
            buildInternalConnexion(graphAudioTrackNode.get(), graphVolumeNode.get(), graphNode->getFormat());
        }
    }
    else if (graphNode->getType() == GraphNodeType::AuxTrackGraphNode) {
        auto volumeNode = std::make_unique<VolumeNode>(transport, graphNode);
        auto graphVolumeNode = graphRef->addNode(std::move(volumeNode));
        outputNode = graphVolumeNode;

        auto trackPtr = getTrackById(graphNode->getTrackId());
        if (trackPtr != nullptr) {
            auto transportPtr = transport.lock();
            auto sampleRate = transportPtr ? transportPtr->getSampleRate() : 48000.0;
            auto blockSize = transportPtr ? transportPtr->getCurrentBlockSize() : 512;
            auto plugins = createPluginNodes(trackPtr->getPlugins(), sampleRate, blockSize);
            if (!plugins.empty()) {
                inputNode = plugins.front();
                connectPluginsToOutput(graphVolumeNode.get(), plugins, graphNode->getFormat());
            } else {
                inputNode = outputNode;
            }
        } else {
            inputNode = outputNode;
        }
    }
}

void GraphModule::buildInternalConnexion(const AudioProcessorGraph::Node* nodeInput, const AudioProcessorGraph::Node* nodeOutput, const ChannelsFormat format) const {
    auto* graphRef = getGraphRef();
    for (auto i=0; i<ChannelCount(format); i++) {
        graphRef->addConnection({
            { nodeInput->nodeID, i },
            { nodeOutput->nodeID, i }
        });
    }
}


std::weak_ptr<AudioTrack> GraphModule::getAudioTrackById(const String& trackId) const {
    const auto editPtr = edit.lock();
    if (!editPtr)
        return {};

    for (const auto& t : editPtr->getTracks())
    {
        if (t->getId() == trackId && t->getTrackType() == TrackType::Audio)
            return std::dynamic_pointer_cast<AudioTrack>(t);
    }

    return {};
}

std::shared_ptr<Track> GraphModule::getTrackById(const String& trackId) const
{
    const auto editPtr = edit.lock();
    if (!editPtr) {
        return {};
    }

    for (const auto& t : editPtr->getTracks())
    {
        if (t->getId() == trackId) {
            return t;
        }
    }

    return {};
}

AudioProcessorGraph* GraphModule::getGraphRef() const
{
    auto* graphRef = graph.lock().get();
    if (graphRef == nullptr) {
        throw std::runtime_error("GraphNode is nullptr");
    }
    return graphRef;
}

std::vector<juce::AudioProcessorGraph::Node::Ptr> GraphModule::createPluginNodes(
    const std::vector<std::shared_ptr<Plugin>>& plugins,
    double sampleRate,
    int blockSize)
{
    std::vector<juce::AudioProcessorGraph::Node::Ptr> nodes;
    if (pluginFactory == nullptr) {
        return nodes;
    }

    for (const auto& plugin : plugins) {
        if (!plugin) {
            continue;
        }
        juce::String error;
        auto instance = pluginFactory->createProcessorInstance(*plugin, sampleRate, blockSize, error);
        if (!instance) {
            juce::Logger::writeToLog("Plugin load error: " + error);
            continue;
        }
        auto node = getGraphRef()->addNode(std::move(instance));
        if (node != nullptr) {
            nodes.push_back(node);
            pluginNodes.push_back(node);
            if (pluginInstanceStore != nullptr) {
                pluginInstanceStore->add({ virtualGraphNode->getTrackId(), plugin->getName(), node });
            }
        }
    }

    return nodes;
}

void GraphModule::connectPluginChain(
    const AudioProcessorGraph::Node* inputNode,
    const AudioProcessorGraph::Node* outputNode,
    const std::vector<juce::AudioProcessorGraph::Node::Ptr>& plugins,
    ChannelsFormat format) const
{
    if (plugins.empty()) {
        buildInternalConnexion(inputNode, outputNode, format);
        return;
    }

    buildInternalConnexion(inputNode, plugins.front().get(), format);
    for (size_t i = 1; i < plugins.size(); ++i) {
        buildInternalConnexion(plugins[i - 1].get(), plugins[i].get(), format);
    }
    buildInternalConnexion(plugins.back().get(), outputNode, format);
}

void GraphModule::connectPluginsToOutput(
    const AudioProcessorGraph::Node* outputNode,
    const std::vector<juce::AudioProcessorGraph::Node::Ptr>& plugins,
    ChannelsFormat format) const
{
    if (plugins.empty()) {
        return;
    }

    for (size_t i = 1; i < plugins.size(); ++i) {
        buildInternalConnexion(plugins[i - 1].get(), plugins[i].get(), format);
    }
    buildInternalConnexion(plugins.back().get(), outputNode, format);
}
