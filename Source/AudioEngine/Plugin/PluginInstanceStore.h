#pragma once

#include <JuceHeader.h>

/// Store and query live plugin instances in the runtime graph.
class PluginInstanceStore {
public:
    /// Plugin parameter snapshot for UI and tooling.
    struct ParamInfo {
        String id;
        String name;
        float minValue = 0.0f;
        float maxValue = 1.0f;
        float defaultValue = 0.0f;
        float value = 0.0f;
    };

    /// PluginInstance describing a plugin instance in the graph.
    struct PluginInstance {
        String trackId;
        String pluginName;
        juce::AudioProcessorGraph::Node::Ptr node;
    };

    /// Register a plugin instance pluginInstance.
    /// @param pluginInstance plugin instance metadata
    void add(PluginInstance pluginInstance) { pluginInstances.push_back(std::move(pluginInstance)); }

    /// Remove all stored plugin instances (call when rebuilding the graph).
    void clear() { pluginInstances.clear(); }

    /// Find a plugin node by track id and name.
    /// @param trackId track id owning the plugin
    /// @param pluginName plugin display name
    juce::AudioProcessorGraph::Node::Ptr find(const String& trackId, const String& pluginName) const;

    /// Find the first plugin node by name.
    /// @param pluginName plugin display name
    juce::AudioProcessorGraph::Node::Ptr findByPluginName(const String& pluginName) const;

    /// Collect current parameter snapshots for a plugin.
    /// @param pluginName plugin display name
    std::vector<ParamInfo> getParameters(const String& pluginName) const;

    /// Find a parameter snapshot by name.
    /// @param pluginName plugin display name
    /// @param paramName parameter display name
    ParamInfo getParameterByName(const String& pluginName, const String& paramName) const;

    /// Read a parameter value by id or name.
    /// @param pluginName plugin display name
    /// @param paramId parameter id or name
    float getParameterValue(const String& pluginName, const String& paramId) const;

    /// Set a parameter value by id or name.
    /// @param pluginName plugin display name
    /// @param paramId parameter id or name
    /// @param value new parameter value (real units)
    bool setParameterValue(const String& pluginName, const String& paramId, float value) const;

private:
    std::vector<PluginInstance> pluginInstances;
};
