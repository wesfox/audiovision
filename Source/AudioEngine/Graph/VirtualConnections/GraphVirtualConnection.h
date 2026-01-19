#pragma once

#include <JuceHeader.h>

#include "../GraphNode.h"

class GraphVirtualConnection {
public:
    GraphVirtualConnection(GraphNode * inputNode, GraphNode * outputNode);

    String id;

    String inputId;
    GraphNode * inputNode;
    String inputName;

    String outputId;
    GraphNode * outputNode;
    String outputName;

    [[nodiscard]] String toString() const {
        return "Input Node : " + inputName + "(" + inputId + ") -> Output Node : " + outputName + "(" + outputId + ")";
    }

private:
};
