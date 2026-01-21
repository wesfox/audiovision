#include "PluginInstanceStore.h"

juce::AudioProcessorGraph::Node::Ptr PluginInstanceStore::find(const String& trackId,
                                                               const String& pluginName) const
{
    for (const auto& entry : entries) {
        if (entry.trackId == trackId && entry.pluginName == pluginName) {
            return entry.node;
        }
    }

    return {};
}
