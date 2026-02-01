#pragma once

namespace juce {
class File;
class Image;
}

/// Backend interface for platform video playback.
class VideoBackend {
public:
    virtual ~VideoBackend() = default;

    /// Load a video file.
    /// @param file file to load
    virtual void loadFile(const juce::File& file) = 0;

    /// Seek to a time in seconds.
    /// @param seconds time to display
    virtual void setPlayheadSeconds(double seconds) = 0;
    virtual void setFrameRate(double frameRate) = 0;

    /// Read the current play position in seconds.
    virtual double getPlayheadSeconds() const = 0;

    /// Start playback.
    virtual void play() = 0;

    /// Stop playback.
    virtual void stop() = 0;

    /// Check readiness.
    virtual bool isReady() const = 0;

    /// Check playback status.
    virtual bool isPlaying() const = 0;

    /// Fetch the current frame image.
    virtual juce::Image getCurrentFrameImage() = 0;

    /// Read the last delivered frame index.
    virtual int64_t getLastFrameIndex() const = 0;

    /// Read the nominal frame rate reported by the backend.
    virtual double getNominalFrameRate() const = 0;
};
