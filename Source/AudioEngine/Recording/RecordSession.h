#pragma once

#include <JuceHeader.h>

#include "RecordState.h"

#include <unordered_map>

class AudioTrack;
class AudioTrackNode;
#include "Recorder.h"

class RecordSession {
public:
    explicit RecordSession(int maxRecordSeconds = 300);

    void registerTrackNode(const String& trackId, AudioTrackNode* node);
    void unregisterTrackNode(const String& trackId);
    void clearTrackNodes();

    void begin(int64 startSample, double sampleRate, int blockSize);
    void end(int64 endSample);

    const RecordState& getState() const { return state; }

private:
    struct ActiveRecorder {
        String trackId;
        std::weak_ptr<AudioTrack> track;
        std::unique_ptr<Recorder> recorder;
    };

    std::unordered_map<String, AudioTrackNode*> trackNodes;
    std::vector<ActiveRecorder> activeRecorders;
    RecordState state;
    int maxRecordSeconds = 300;
    double sampleRate = 0.0;
    int blockSize = 0;

    juce::File recordingsDirectory() const;
    void finalizeRecorder(const ActiveRecorder& activeRecorder);
};
