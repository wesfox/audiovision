#pragma once

#include <JuceHeader.h>

#include "AudioEngine/Graph/Builder/GraphBuilder.h"

class GraphMermaidExporter {
public:
    /// static function to use in order to get a mermaid export of the current session state (in the GraphDescription
    /// format). This description is aimed to be transformed into an AudioProcessorGraph.
    /// @param description a reference to the GraphDescription to export
    static juce::String exportMermaid(const GraphDescription& description);
};
