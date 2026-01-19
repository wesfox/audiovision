#include "GraphVirtualConnection.h"

// ------------------------ MainComponent Implementation ------------------------

GraphVirtualConnection::GraphVirtualConnection(GraphNode * inputNode, GraphNode * outputNode) : inputNode(inputNode),
    outputNode(outputNode) {
    if (inputNode != nullptr) {
        inputName = inputNode->getName();
        inputId = inputNode->getId();
    }if (outputNode != nullptr) {
        outputName = outputNode->getName();
        outputId = outputNode->getId();
    }
}