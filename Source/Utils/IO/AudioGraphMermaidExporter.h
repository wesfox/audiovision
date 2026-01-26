#pragma once

#include <JuceHeader.h>

/// Export an AudioProcessorGraph to Mermaid syntax.
class AudioGraphMermaidExporter {
public:
    /// Export a graph to Mermaid text (for debugging).
    /// @param graph audio processor graph to export
    static juce::String exportMermaid(const juce::AudioProcessorGraph& graph);
};
