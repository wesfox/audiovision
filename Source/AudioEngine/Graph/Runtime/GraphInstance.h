#pragma once

#include <JuceHeader.h>

#include "GraphManager.h"
#include "AudioEngine/Recording/RecordSession.h"

/// Runtime graph built from an Edit, with recording support.
class GraphInstance {
public:
    /// Create a graph instance bound to an edit and transport.
    /// @param edit edit containing tracks and routing
    /// @param transport shared transport for playback/recording
    GraphInstance(
        const std::shared_ptr<Edit>& edit,
        const std::shared_ptr<Transport>& transport);

    /// Build the graph nodes and connections (call after construction).
    void build() const;

    /// Prepare graph nodes for audio playback.
    /// @param sampleRate current engine sample rate
    /// @param blockSize current engine block size
    void prepareToPlay(double sampleRate, int blockSize);

    /// Release resources and clear the graph.
    void shutdown();

    /// Begin a record pass for all registered track nodes.
    void startRecording();

    /// End the current record pass.
    void stopRecording();

    /// Access the underlying audio processor graph.
    juce::AudioProcessorGraph& getGraph() const { return *graph; }

    /// Access the graph manager (routing/build info).
    GraphManager& getGraphManager() const { return *graphManager; }

    /// Access the record session for this graph.
    RecordSession& getRecordSession() const { return *recordSession; }

    /// Process audio through the graph.
    /// @param buffer audio buffer to process
    /// @param midi midi buffer to process
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi);

private:
    std::shared_ptr<Edit> edit;
    std::shared_ptr<Transport> transport;
    std::shared_ptr<juce::AudioProcessorGraph> graph;
    std::unique_ptr<GraphManager> graphManager;
    std::unique_ptr<RecordSession> recordSession;
};
