#pragma once

#include <JuceHeader.h>

#include "Utils/Format.h"

class Plugin;
class PluginInstanceFactory;
class PluginInstanceStore;

/// Build and connect plugin nodes inside an AudioProcessorGraph.
class PluginChainBuilder {
public:
    /// Create a chain builder bound to a plugin factory and store.
    /// @param pluginFactory factory used to instantiate plugins
    /// @param pluginInstanceStore store for runtime plugin nodes
    PluginChainBuilder(PluginInstanceFactory* pluginFactory,
                       PluginInstanceStore* pluginInstanceStore);

    /// Create graph nodes for a plugin list.
    /// @param plugins list of plugins to instantiate
    /// @param graph target audio processor graph
    /// @param trackId track id owning the plugins
    /// @param sampleRate current engine sample rate
    /// @param blockSize current engine block size
    std::vector<juce::AudioProcessorGraph::Node::Ptr> createPluginNodes(
        const std::vector<std::shared_ptr<Plugin>>& plugins,
        juce::AudioProcessorGraph& graph,
        const String& trackId,
        double sampleRate,
        int blockSize) const;

    /// Connect input -> plugins -> output for a track.
    /// @param graph target graph
    /// @param inputNode upstream node
    /// @param outputNode downstream node
    /// @param plugins plugin nodes to connect
    /// @param format channel format to connect
    void connectChain(juce::AudioProcessorGraph& graph,
                      const juce::AudioProcessorGraph::Node* inputNode,
                      const juce::AudioProcessorGraph::Node* outputNode,
                      const std::vector<juce::AudioProcessorGraph::Node::Ptr>& plugins,
                      ChannelsFormat format) const;

    /// Connect plugins to a single output node.
    /// @param graph target graph
    /// @param outputNode downstream node
    /// @param plugins plugin nodes to connect
    /// @param format channel format to connect
    void connectPluginsToOutput(juce::AudioProcessorGraph& graph,
                                const juce::AudioProcessorGraph::Node* outputNode,
                                const std::vector<juce::AudioProcessorGraph::Node::Ptr>& plugins,
                                ChannelsFormat format) const;

private:
    /// Connect two nodes for all channels in the format.
    /// @param graph target graph
    /// @param nodeInput upstream node
    /// @param nodeOutput downstream node
    /// @param format channel format to connect
    static void connectNodes(juce::AudioProcessorGraph& graph,
                             const juce::AudioProcessorGraph::Node* nodeInput,
                             const juce::AudioProcessorGraph::Node* nodeOutput,
                             ChannelsFormat format);

    PluginInstanceFactory* pluginFactory;
    PluginInstanceStore* pluginInstanceStore;
};
