#include "PluginInstanceFactory.h"

#include <juce_audio_processors/format/juce_AudioPluginFormatManagerHelpers.h>

PluginInstanceFactory::PluginInstanceFactory()
{
    juce::addDefaultFormatsToManager(formatManager);
}

std::unique_ptr<juce::AudioPluginInstance> PluginInstanceFactory::createProcessorInstance(const Plugin& plugin,
                                                                                          double sampleRate,
                                                                                          int blockSize,
                                                                                          juce::String& error) const
{
    error.clear();

    if (plugin.getFormat() != PluginFormat::VST3) {
        error = "Unsupported plugin format";
        return nullptr;
    }

    juce::AudioPluginFormat* format = nullptr;
    for (int i = 0; i < formatManager.getNumFormats(); ++i) {
        auto* candidate = formatManager.getFormat(i);
        if (candidate != nullptr && candidate->getName().containsIgnoreCase("VST3")) {
            format = candidate;
            break;
        }
    }
    if (format == nullptr) {
        error = "VST3 format not available";
        return nullptr;
    }

    juce::OwnedArray<juce::PluginDescription> descriptions;
    format->findAllTypesForFile(descriptions, plugin.getPath());
    if (descriptions.isEmpty()) {
        error = "No plugin description found";
        return nullptr;
    }

    auto instance = formatManager.createPluginInstance(
        *descriptions[0],
        sampleRate,
        blockSize,
        error);
    if (instance == nullptr) {
        if (error.isEmpty()) {
            error = "Failed to create plugin instance";
        }
        return nullptr;
    }

    return instance;
}

std::shared_ptr<PluginRuntime> PluginInstanceFactory::createInstance(const Plugin& plugin,
                                                                     double sampleRate,
                                                                     int blockSize,
                                                                     juce::String& error) const
{
    auto instance = createProcessorInstance(plugin, sampleRate, blockSize, error);
    if (!instance) {
        return {};
    }
    return std::make_shared<PluginRuntime>(std::move(instance));
}
