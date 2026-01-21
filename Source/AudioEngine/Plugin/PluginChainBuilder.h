#pragma once

#include <JuceHeader.h>

#include "Utils/Format.h"

class Plugin;
class PluginInstanceFactory;
class PluginInstanceStore;

class PluginChainBuilder {
public:
    PluginChainBuilder(PluginInstanceFactory* pluginFactory,
                       PluginInstanceStore* pluginInstanceStore);

    std::vector<juce::AudioProcessorGraph::Node::Ptr> createPluginNodes(
        const std::vector<std::shared_ptr<Plugin>>& plugins,
        juce::AudioProcessorGraph& graph,
        const String& trackId,
        double sampleRate,
        int blockSize) const;

    void connectChain(juce::AudioProcessorGraph& graph,
                      const juce::AudioProcessorGraph::Node* inputNode,
                      const juce::AudioProcessorGraph::Node* outputNode,
                      const std::vector<juce::AudioProcessorGraph::Node::Ptr>& plugins,
                      ChannelsFormat format) const;

    void connectPluginsToOutput(juce::AudioProcessorGraph& graph,
                                const juce::AudioProcessorGraph::Node* outputNode,
                                const std::vector<juce::AudioProcessorGraph::Node::Ptr>& plugins,
                                ChannelsFormat format) const;

private:
    static void connectNodes(juce::AudioProcessorGraph& graph,
                             const juce::AudioProcessorGraph::Node* nodeInput,
                             const juce::AudioProcessorGraph::Node* nodeOutput,
                             ChannelsFormat format);

    PluginInstanceFactory* pluginFactory;
    PluginInstanceStore* pluginInstanceStore;
};
