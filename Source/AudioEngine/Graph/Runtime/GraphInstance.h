#pragma once

#include <JuceHeader.h>

#include "GraphManager.h"

class GraphInstance {
public:
    GraphInstance(
        const std::shared_ptr<Edit>& edit,
        const std::shared_ptr<Transport>& transport,
        int sceneId=0);

    void build();
    void prepareToPlay(double sampleRate, int blockSize);

    juce::AudioProcessorGraph& getGraph() const { return *graph; }
    GraphManager& getGraphManager() const { return *graphManager; }

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi);

    auto& getScene() const noexcept { return edit->getScenes().at(sceneId); }

private:
    int sceneId;
    std::shared_ptr<Edit> edit;
    std::shared_ptr<Transport> transport;
    std::shared_ptr<juce::AudioProcessorGraph> graph;
    std::unique_ptr<GraphManager> graphManager;
};
