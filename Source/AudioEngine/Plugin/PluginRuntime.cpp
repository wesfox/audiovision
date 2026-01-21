#include "PluginRuntime.h"

PluginRuntime::PluginRuntime(std::unique_ptr<juce::AudioPluginInstance> instance)
    : pluginInstance(std::move(instance))
{
}

juce::AudioProcessorEditor* PluginRuntime::createEditor() const
{
    if (pluginInstance == nullptr) {
        return nullptr;
    }
    return pluginInstance->createEditor();
}
