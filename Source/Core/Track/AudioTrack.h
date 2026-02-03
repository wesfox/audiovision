#pragma once

#include <JuceHeader.h>
#include "Track.h"
#include <Core/AudioClip/AudioClip.h>

/// Track that owns and renders audio clips.
class AudioTrack : public Track {
public:
    /// Create an audio track with an optional name.
    /// @param name display name
    AudioTrack(const String &name="");

    /// Create a shared audio track.
    /// @param name display name
    static std::shared_ptr<AudioTrack> create(const String& name="")
    {
        return std::make_shared<AudioTrack>(name);
    }

    /// True when the track monitors input instead of clips.
    bool isInputMonitoring() const {
        return getInputMonitoringState() == TrackInputMonitoringState::Active;
    }

    /// Adds an audioClip to the track. If the added clip overlaps an already existing clip on this track,
    /// (based on its startSample / endSample), the overlapped clip is split.
    /// @see AudioClip::split()
    void addAudioClip(std::unique_ptr<AudioClip> audioClip) {
        audioClips.push_back(std::move(audioClip));
    }

    /// True when recording will create a new clip.
    /// @see addAudioClip()
    /// @see RecordManager()
    bool isArmed() const {
        return getArmState() == TrackArmState::Active;
    }

    /// Arm or disarm this track for recording.
    /// @param shouldArm new armed state
    void setArmed(bool shouldArm) {
        setArmState(shouldArm ? TrackArmState::Active : TrackArmState::Inactive);
    }

    /// Access clips owned by this track.
    const std::vector<std::unique_ptr<AudioClip>>& getAudioClips() const noexcept{
        return audioClips;
    }

private:

    std::vector<std::unique_ptr<AudioClip>> audioClips;
    // Recorder recorder;
};
