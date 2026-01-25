#pragma once

#include <JuceHeader.h>

class PluginInstanceStore {
public:
    struct ParamInfo {
        String id;
        String name;
        float minValue = 0.0f;
        float maxValue = 1.0f;
        float defaultValue = 0.0f;
        float value = 0.0f;
    };

    struct Entry {
        String trackId;
        String pluginName;
        juce::AudioProcessorGraph::Node::Ptr node;
    };

    void add(Entry entry) { entries.push_back(std::move(entry)); }
    void clear() { entries.clear(); }

    juce::AudioProcessorGraph::Node::Ptr find(const String& trackId, const String& pluginName) const;
    juce::AudioProcessorGraph::Node::Ptr findByPluginName(const String& pluginName) const;
    std::vector<ParamInfo> getParameters(const String& pluginName) const;
    ParamInfo getParameterByName(const String& pluginName, const String& paramName) const;
    float getParameterValue(const String& pluginName, const String& paramId) const;
    bool setParameterValue(const String& pluginName, const String& paramId, float value) const;

private:
    std::vector<Entry> entries;
};
