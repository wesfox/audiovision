#pragma once

#include <JuceHeader.h>

#include "RecordState.h"

#include <unordered_map>

class AudioTrack;
class AudioTrackNode;
#include "Recorder.h"

/// Orchestrates a single recording pass across multiple track nodes.
class RecordSession {
public:
    explicit RecordSession(int maxRecordSeconds = 300);

    /// Attach a track node to receive recorder assignment during a take.
    /// @param trackId stable track identifier
    /// @param node runtime node used to inject a Recorder while recording
    void registerTrackNode(const String& trackId, AudioTrackNode* node);

    /// Detach a track node from the session.
    /// @param trackId stable track identifier
    void unregisterTrackNode(const String& trackId);

    /// Detach all track nodes from the recording session.
    void clearTrackNodes();

    /// Start a record pass at the given timeline sample.
    /// @param startSample absolute timeline sample where recording starts
    /// @param sampleRate current engine sample rate
    /// @param blockSize current engine block size
    void begin(int64 startSample, double sampleRate, int blockSize);

    /// End the current record pass at the given timeline sample.
    /// @param endSample absolute timeline sample where recording ends
    void end(int64 endSample);

    /// Current record state snapshot.
    const RecordState& getState() const { return state; }

private:
    /// Active recorder tied to a track while recording.
    struct ActiveRecorder {
        String trackId;
        std::weak_ptr<AudioTrack> track;
        std::unique_ptr<Recorder> recorder;
    };

    /// Registered audio track nodes by track id.
    std::unordered_map<String, AudioTrackNode*> trackNodes;
    /// Recorders currently capturing data.
    std::vector<ActiveRecorder> activeRecorders;
    RecordState state;
    int maxRecordSeconds = 300;
    double sampleRate = 0.0;
    int blockSize = 0;

    /// Resolve the target folder for recording files.
    juce::File recordingsDirectory() const;

    /// Finish and write a recorder to disk.
    /// @param activeRecorder recorder instance to finalize
    void finalizeRecorder(const ActiveRecorder& activeRecorder);
};
