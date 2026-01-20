#pragma once

#include <JuceHeader.h>

class AudioGraphMermaidExporter {
public:
    /// static function to use in order to get a mermaid export of the AudioProcessorGraph passed as a parameter.
    /// @param graph a reference to the AudioProcessorGraph to export
    static juce::String exportMermaid(const juce::AudioProcessorGraph& graph);
};
