#pragma once

#include <JuceHeader.h>

#include "GraphManager.h"
#include "AudioEngine/Recording/RecordSession.h"

class GraphInstance {
public:
    GraphInstance(
        const std::shared_ptr<Edit>& edit,
        const std::shared_ptr<Transport>& transport);

    void build() const;
    void prepareToPlay(double sampleRate, int blockSize);
    void shutdown();
    void startRecording();
    void stopRecording();

    juce::AudioProcessorGraph& getGraph() const { return *graph; }
    GraphManager& getGraphManager() const { return *graphManager; }
    RecordSession& getRecordSession() const { return *recordSession; }

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi);

private:
    std::shared_ptr<Edit> edit;
    std::shared_ptr<Transport> transport;
    std::shared_ptr<juce::AudioProcessorGraph> graph;
    std::unique_ptr<GraphManager> graphManager;
    std::unique_ptr<RecordSession> recordSession;
};
