#pragma once

#include <JuceHeader.h>

#include "Core/Plugin/Plugin.h"

/// Create plugin instances from plugin metadata.
class PluginInstanceFactory {
public:
    PluginInstanceFactory();

    /// Instantiate a plugin processor from its description.
    /// @param plugin plugin metadata
    /// @param sampleRate current engine sample rate
    /// @param blockSize current engine block size
    /// @param error output error message (empty on success)
    std::unique_ptr<juce::AudioPluginInstance> createProcessorInstance(const Plugin& plugin,
                                                                       double sampleRate,
                                                                       int blockSize,
                                                                       juce::String& error) const;
private:
    juce::AudioPluginFormatManager formatManager;
};
