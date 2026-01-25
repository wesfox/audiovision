#pragma once

#include <JuceHeader.h>

#include "Core/Plugin/Plugin.h"

class PluginInstanceFactory {
public:
    PluginInstanceFactory();

    std::unique_ptr<juce::AudioPluginInstance> createProcessorInstance(const Plugin& plugin,
                                                                       double sampleRate,
                                                                       int blockSize,
                                                                       juce::String& error) const;
private:
    juce::AudioPluginFormatManager formatManager;
};
