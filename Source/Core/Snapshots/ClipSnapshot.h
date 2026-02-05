#pragma once

#include <JuceHeader.h>

#include "Core/Track/Track.h"
#include "Utils/Format.h"

class AudioClip;

/// Serialized clip data for undo and restore workflows.
struct ClipData {
    String audioFilePath;
    int64 fileStartSample = 0;
    int64 sessionStartSample = 0;
    int64 sessionEndSample = 0;
    double sampleRate = 0.0;
    ChannelsFormat format = ChannelsFormat::Mono;
    bool muted = false;
    String name;
    juce::Colour color = juce::Colours::white;
    bool fadeInActive = false;
    int fadeInEndSample = 0;
    bool fadeOutActive = false;
    int fadeOutStartSample = 0;
};

/// Snapshot of a clip and its owning track.
struct ClipSnapshot {
    String trackId;
    String clipId;
    ClipData data;
};

/// Serialize a clip for undo workflows.
/// @param clip clip to serialize
/// @param track owning track
ClipSnapshot serializeClip(const AudioClip& clip, const Track& track);

/// Restore a clip from a snapshot.
/// @param snapshot snapshot to restore
std::unique_ptr<AudioClip> restoreClip(const ClipSnapshot& snapshot);
