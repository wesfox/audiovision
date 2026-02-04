#pragma once

#include <JuceHeader.h>
#include "Track.h"
#include <Core/AudioClip/AudioClip.h>

/// Track that owns and renders audio clips.
class AudioTrack : public Track {
public:
    /// Notifies listeners about clip changes.
    class Listener {
    public:
        virtual ~Listener() = default;

        /// Called when clips change on this track.
        virtual void clipsChanged(AudioTrack& track) = 0;
    };

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
        notifyClipsChanged();
    }

    /// Split clips that contain the sample.
    /// @param splitSample absolute session sample to split at
    void splitClipsAtSample(int64 splitSample);

    /// Merge compatible clips whose boundaries fall within a range.
    /// @param rangeStart range start in session samples
    /// @param rangeEnd range end in session samples
    void healClipsInRange(int64 rangeStart, int64 rangeEnd);

    /// Delete or split clips that intersect a range.
    /// @param rangeStart range start in session samples
    /// @param rangeEnd range end in session samples
    void deleteClipsInRange(int64 rangeStart, int64 rangeEnd);

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

    /// Register a clip listener.
    /// @param listener listener to add
    void addListener(Listener* listener) {
        listeners.add(listener);
    }

    /// Remove a clip listener.
    /// @param listener listener to remove
    void removeListener(Listener* listener) {
        listeners.remove(listener);
    }

private:
    void notifyClipsChanged() {
        listeners.call([this](Listener& listener) { listener.clipsChanged(*this); });
    }

    std::vector<std::unique_ptr<AudioClip>> audioClips;
    juce::ListenerList<Listener> listeners;
    // Recorder recorder;
};
