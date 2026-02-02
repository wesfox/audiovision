#pragma once

#include <JuceHeader.h>

#include "Core/Video/VideoFile.h"
#include "Gui/Video/Backend/VideoBackend.h"

/// Render video frames via a platform backend.
class VideoRenderer : public juce::Component {
public:
    /// Create a renderer with a platform backend.
    VideoRenderer();

    /// Load a video file into the renderer.
    /// @param file video file to load
    void setVideoFile(const VideoFile& file);

    /// Seek to a time in seconds.
    /// @param seconds time to display
    void setPositionSeconds(double seconds);

    /// Seek to a time without forcing a refresh.
    /// @param seconds time to display
    void setPlayheadSeconds(double seconds);

    /// Update the frame rate used by the backend.
    /// @param frameRate frames per second
    void setFrameRate(double frameRate);

    /// Read the current play position in seconds.
    double getPlayPositionSeconds() const;

    /// Start playback.
    void play();

    /// Stop playback.
    void stop();

    /// Check whether the backend is ready.
    bool isReady() const;

    /// Check whether the backend is playing.
    bool isPlaying() const;

    /// Refresh the displayed frame from the backend.
    void refreshFrame();

    /// Read the last delivered frame index.
    int64_t getLastFrameIndex() const;

    /// Read the nominal frame rate reported by the backend.
    double getNominalFrameRate() const;

    /// Override the displayed frame with a preview image.
    /// @param frame still image to display
    void setPreviewFrame(const juce::Image& frame);

    void paint(juce::Graphics& g) override;

private:
    std::unique_ptr<VideoBackend> backend;
    juce::File currentFile;
    juce::Image currentFrame;
};
