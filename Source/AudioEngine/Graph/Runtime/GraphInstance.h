#pragma once

#include <JuceHeader.h>

#include "GraphManager.h"

class GraphInstance {
public:
    GraphInstance(
        const std::shared_ptr<Edit>& edit,
        const std::shared_ptr<Transport>& transport,
        std::weak_ptr<Scene> scene);

    void build() const;
    void prepareToPlay(double sampleRate, int blockSize);
    void shutdown();

    juce::AudioProcessorGraph& getGraph() const { return *graph; }
    GraphManager& getGraphManager() const { return *graphManager; }

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi);

    Scene* getScene() const {
        if (const auto sceneRef = scene.lock()) {
            return sceneRef.get();
        }
        return {};
    }

private:
    std::shared_ptr<Edit> edit;
    std::shared_ptr<Transport> transport;
    std::shared_ptr<juce::AudioProcessorGraph> graph;
    std::unique_ptr<GraphManager> graphManager;
    std::weak_ptr<Scene> scene;
};
