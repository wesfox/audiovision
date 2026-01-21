#pragma once

#include <JuceHeader.h>

class PluginInstanceStore {
public:
    struct Entry {
        String trackId;
        String pluginName;
        juce::AudioProcessorGraph::Node::Ptr node;
    };

    void add(Entry entry) { entries.push_back(std::move(entry)); }
    void clear() { entries.clear(); }

    juce::AudioProcessorGraph::Node::Ptr find(const String& trackId, const String& pluginName) const;

private:
    std::vector<Entry> entries;
};
