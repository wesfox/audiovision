#pragma once

#include <JuceHeader.h>
#include "Track.h"
#include <Core/AudioClip/AudioClip.h>
#include "Utils/IO/Recorder.h"

class AudioTrack : public Track {
public:
    AudioTrack(const String &name="");
    ~AudioTrack() = default;

    /// This function returns a new AudioTrack
    static std::shared_ptr<AudioTrack> create(const String& name="")
    {
        return std::make_shared<AudioTrack>(name);
    }

    /// If true, the audio track works as an aux track and ignores its clips
    bool isInputMonitoring() const {
        return inputMonitoring;
    }

    /// Adds an audioClip to the track. If the added clip overlaps an already existing clip on this track,
    /// (based on its startSample / endSample), the overlapped clip is split.
    /// @see AudioClip::split()
    void addAudioClip(std::unique_ptr<AudioClip> audioClip) {
        audioClips.push_back(std::move(audioClip));
    }

    /// If true, if a record start, a new audio clip will be created at the end of the record calling addAudioClip()
    /// @see addAudioClip()
    /// @see RecordManager()
    bool isArmed() const {
        return armed;
    }

    /// Returns the list of Clips owned by the AudioTrack
    const std::vector<std::unique_ptr<AudioClip>>& getAudioClips() const noexcept{
        return audioClips;
    }

private:

    std::vector<std::unique_ptr<AudioClip>> audioClips;

    bool inputMonitoring;
    bool armed;
    Recorder recorder;
};
