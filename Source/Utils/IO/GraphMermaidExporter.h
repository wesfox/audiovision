#pragma once

#include <JuceHeader.h>

#include "AudioEngine/Graph/Builder/GraphBuilder.h"

/// Export a graph description to Mermaid syntax.
class GraphMermaidExporter {
public:
    /// Export a graph description to Mermaid text (for debugging).
    /// @param description graph description to export
    static juce::String exportMermaid(const GraphDescription& description);
};
