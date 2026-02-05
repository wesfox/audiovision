#pragma once

#include <JuceHeader.h>
#include <Utils/Format.h>
#include <Utils/IO/AudioFile.h>
#include "FadeIn.h"
#include "FadeOut.h"
#include "GainLine.h"

/// Audio clip with timing, fades, and gain automation.
class AudioClip {
public:
    /// Create a clip bound to an audio file.
    /// @param audioFile source audio file
    AudioClip(const std::shared_ptr<AudioFile> &audioFile);

    /// Create a clip with default timing.
    /// @param audioFile source audio file
    static std::unique_ptr<AudioClip> create(const std::shared_ptr<AudioFile>& audioFile)
    {
        return std::make_unique<AudioClip>(audioFile);
    }

    /// Create a clip with explicit timing.
    /// @param audioFile source audio file
    /// @param fileStartSample start sample within the file
    /// @param sessionStartSample start sample in the session timeline
    /// @param sessionEndSample end sample in the session timeline
    static std::unique_ptr<AudioClip> create(const std::shared_ptr<AudioFile>& audioFile,
                                             int64 fileStartSample,
                                             int64 sessionStartSample,
                                             int64 sessionEndSample)
    {
        auto clip = std::make_unique<AudioClip>(audioFile);
        clip->setFileStartSample(fileStartSample);
        clip->setSessionStartSample(sessionStartSample);
        clip->setSessionEndSample(sessionEndSample);
        return clip;
    }

    ~AudioClip() = default;

    /// Clip identifier.
    String getId() const;

    /// Clip display name.
    String getName() const;

    /// Set the clip display name.
    /// @param newName display name
    void setName(const String& newName);

    /// Clip color used in the UI.
    juce::Colour getColor() const;

    /// Set the clip color used in the UI.
    /// @param newColor new color value
    void setColor(juce::Colour newColor);

    /// Source audio file path.
    String getAudioFilePath() const;

    /// Access the source audio file.
    std::shared_ptr<AudioFile> getAudioFile() const;

    /// Start sample within the audio file.
    int64 getFileStartSample() const;

    /// Start sample in the session timeline.
    int64 getSessionStartSample() const;

    /// End sample of the Clip, when reading, only [startSample, endSample - 1] are read
    int64 getSessionEndSample() const;

    /// Clip length in samples.
    int64 getClipLength() const;

    /// Sample rate of the source file.
    double getSampleRate() const;

    /// Channel format of the clip.
    ChannelsFormat getFormat() const;

    /// Set the channel format of the clip.
    /// @param newFormat new format
    void setFormat(ChannelsFormat newFormat);

    /// Compute waveform data for display.
    std::vector<float> getWaveformData() const;

    /// Reads a chunk of the Clip
    /// @param startSample positive integer, relative start sample FROM the beginning of the Clip
    /// @param numberOfSamples number of samples to read. If the length of the Clip is < startSample + numberOfSamples (the clips ends before), it is filled with zeros
    juce::AudioBuffer<float> read(int64 startSample, int64 numberOfSamples) const;

    /// Move the clip to a new start sample in the timeline (keeping its exact length)
    /// @param newSessionStartSample positive integer, absolute new position of the clip start sample
    void move(int64 newSessionStartSample);

    /// Create a new clip, starting from splitSample to the end of the current clip, returning the created Clip
    /// Current clip sets its new end to this splitSample.
    /// New clip start at splitSample and ends at current endSample.
    /// Gain Line is split accordingly
    /// Fades are kept
    /// @param splitSample positive integer, absolute (timeline) new position of the cut
    std::unique_ptr<AudioClip> split(int64 splitSample);

    /// Change the clip' start sample, keeping its end, changing the length of the clip, Fade behavior is based on
    /// trimFade.
    /// If keepFade is true, fadeIn is kept, limited to the tail of the clip, and moving the FadeOut if needed.
    /// If keepFade is false, fadeIn is deleted if it gets out of bounds.
    /// Gain Line is trimmed accordingly (but kept in cache), inverse operation make it appear unchanged.
    /// newSessionsStartSample min value is the first sample of the AudioFile (keeping the length)
    /// @param newSessionStartSample the new start sample of the clip
    /// @param keepFade si the fade also trimmed or moved with the head
    void trimHead(int64 newSessionStartSample, bool keepFade = false);

    /// Change the clip' end sample, keeping its start, changing the length of the clip, Fade behavior is based on
    /// trimFade.
    /// If keepFade is true, fadeOut is kept, limited to the head of the clip, and moving the FadeIn if needed.
    /// If keepFade is false, fadeOut is deleted if it gets out of bounds.
    /// Gain Line is trimmed accordingly (but kept in cache), inverse operation make it appear unchanged.
    /// @param newSessionEndSample the new start sample of the clip
    /// @param keepFade si the fade also trimmed or moved with the head
    void trimTail(int64 newSessionEndSample, bool keepFade = false);

    /// Set the start sample in the session timeline.
    /// @param newSessionStartSample new start sample
    void setSessionStartSample(int64 newSessionStartSample);

    /// Set the end sample in the session timeline.
    /// @param newSessionEndSample new end sample
    void setSessionEndSample(int64 newSessionEndSample);

    /// Set the clip sample rate.
    /// @param newSampleRate new sample rate
    void setSampleRate(double newSampleRate);

    /// Set the start sample within the audio file.
    /// @param newFileStartSample new file start sample
    void setFileStartSample(int64 newFileStartSample);

    /// True when a fade-in is active.
    bool hasFadeIn() const { return fadeIn.isActive(); }

    /// Read the fade-in end sample.
    int getFadeInEndSample() const { return fadeIn.endSample; }

    /// Set the fade-in state.
    /// @param active true to enable the fade
    /// @param endSample end sample in clip-relative samples
    void setFadeIn(bool active, int endSample);

    /// True when a fade-out is active.
    bool hasFadeOut() const { return fadeOut.isActive(); }

    /// Read the fade-out start sample.
    int getFadeOutStartSample() const { return fadeOut.startSample; }

    /// Set the fade-out state.
    /// @param active true to enable the fade
    /// @param startSample start sample in clip-relative samples
    void setFadeOut(bool active, int startSample);

    /// If true, the read method returns an empty (cleared), buffer
    bool isMuted() const { return muted; }

    /// Mute or unmute the clip.
    /// @param isMuted new mute state
    void mute(const bool isMuted){ muted = isMuted; }

    /// Override the clip id for restore workflows.
    /// @param newId restored clip id
    void setIdForRestore(const String& newId);

private:
    String id;

    int64 fileStartSample;
    int64 sessionStartSample;
    int64 sessionEndSample;
    int64 clipLength;
    double sampleRate;
    bool muted;

    std::shared_ptr<AudioFile> audioFile;
    FadeIn fadeIn;
    FadeOut fadeOut;

    GainLine gainLine;
    ChannelsFormat format;

    String name;
    juce::Colour color;

    mutable std::vector<float> waveformCache;
    mutable int64 waveformCacheLength = -1;
};
