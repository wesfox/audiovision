#include "PluginChainBuilder.h"

#include "AudioEngine/Plugin/PluginInstanceFactory.h"
#include "AudioEngine/Plugin/PluginInstanceStore.h"
#include "Core/Plugin/Plugin.h"

PluginChainBuilder::PluginChainBuilder(PluginInstanceFactory* pluginFactory,
                                       PluginInstanceStore* pluginInstanceStore)
    : pluginFactory(pluginFactory),
      pluginInstanceStore(pluginInstanceStore)
{
}

std::vector<juce::AudioProcessorGraph::Node::Ptr> PluginChainBuilder::createPluginNodes(
    const std::vector<std::shared_ptr<Plugin>>& plugins,
    juce::AudioProcessorGraph& graph,
    const String& trackId,
    double sampleRate,
    int blockSize) const
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
        auto node = graph.addNode(std::move(instance));
        if (node != nullptr) {
            nodes.push_back(node);
            if (pluginInstanceStore != nullptr) {
                pluginInstanceStore->add({ trackId, plugin->getName(), node });
            }
        }
    }

    return nodes;
}

void PluginChainBuilder::connectChain(juce::AudioProcessorGraph& graph,
                                      const juce::AudioProcessorGraph::Node* inputNode,
                                      const juce::AudioProcessorGraph::Node* outputNode,
                                      const std::vector<juce::AudioProcessorGraph::Node::Ptr>& plugins,
                                      ChannelsFormat format) const
{
    if (plugins.empty()) {
        connectNodes(graph, inputNode, outputNode, format);
        return;
    }

    connectNodes(graph, inputNode, plugins.front().get(), format);
    for (size_t i = 1; i < plugins.size(); ++i) {
        connectNodes(graph, plugins[i - 1].get(), plugins[i].get(), format);
    }
    connectNodes(graph, plugins.back().get(), outputNode, format);
}

void PluginChainBuilder::connectPluginsToOutput(juce::AudioProcessorGraph& graph,
                                                const juce::AudioProcessorGraph::Node* outputNode,
                                                const std::vector<juce::AudioProcessorGraph::Node::Ptr>& plugins,
                                                ChannelsFormat format) const
{
    if (plugins.empty()) {
        return;
    }

    for (size_t i = 1; i < plugins.size(); ++i) {
        connectNodes(graph, plugins[i - 1].get(), plugins[i].get(), format);
    }
    connectNodes(graph, plugins.back().get(), outputNode, format);
}

void PluginChainBuilder::connectNodes(juce::AudioProcessorGraph& graph,
                                      const juce::AudioProcessorGraph::Node* nodeInput,
                                      const juce::AudioProcessorGraph::Node* nodeOutput,
                                      ChannelsFormat format)
{
    for (auto i = 0; i < ChannelCount(format); i++) {
        graph.addConnection({
            { nodeInput->nodeID, i },
            { nodeOutput->nodeID, i }
        });
    }
}
