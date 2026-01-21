#pragma once

#include <JuceHeader.h>

class PluginRuntime {
public:
    explicit PluginRuntime(std::unique_ptr<juce::AudioPluginInstance> instance);

    juce::AudioPluginInstance& getProcessor() const { return *pluginInstance; }
    juce::AudioProcessorEditor* createEditor() const;

private:
    std::unique_ptr<juce::AudioPluginInstance> pluginInstance;
};
