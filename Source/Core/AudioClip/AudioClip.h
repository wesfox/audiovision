#pragma once

#include <JuceHeader.h>
#include <Utils/Format.h>
#include <Utils/IO/AudioFile.h>
#include "FadeIn.h"
#include "FadeOut.h"
#include "GainLine.h"

class AudioClip {
public:
    AudioClip(const std::shared_ptr<AudioFile> &audioFile);
    /// This function returns a new AudioClip
    static std::unique_ptr<AudioClip> create(const std::shared_ptr<AudioFile>& audioFile)
    {
        return std::make_unique<AudioClip>(audioFile);
    }

    ~AudioClip() = default;

    // Getters
    String getId() const;
    String getName() const;
    juce::Colour getColor() const;
    int64 getFileStartSample() const;
    int64 getSessionStartSample() const;

    /// End sample of the Clip, when reading, only [startSample, endSample - 1] are read
    int64 getSessionEndSample() const;
    int64 getClipLength() const;
    double getSampleRate() const;
    ChannelsFormat getFormat() const;

    // Methods
    std::vector<float> getWaveformData();

    /// Reads a chunk of the Clip
    /// @param startSample positive integer, relative start sample FROM the beginning of the Clip
    /// @param numberOfSamples number of samples to read. If the length of the Clip is < startSample + numberOfSamples (the clips ends before), it is filled with zeros
    juce::AudioBuffer<float> read(int64 startSample, int64 numberOfSamples);

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

    void setSessionStartSample(int64 newSessionStartSample);
    void setSessionEndSample(int64 newSessionEndSample);
    void setSampleRate(double newSampleRate);

    /// If true, the read method returns an empty (cleared), buffer
    bool isMuted() const { return muted; }
    void mute(const bool isMuted){ muted = isMuted; }

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
};
