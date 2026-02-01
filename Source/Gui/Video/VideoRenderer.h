#pragma once

#include <JuceHeader.h>

#include "Core/Video/VideoFile.h"

/// Render video frames using JUCE video component.
class VideoRenderer {
public:
    /// Create a renderer owning its video component.
    VideoRenderer();

    /// Access the underlying JUCE video component.
    juce::VideoComponent& getComponent();

    /// Load a video file into the renderer.
    /// @param file video file to load
    void setVideoFile(const VideoFile& file);

    /// Seek to a time in seconds.
    /// @param seconds time to display
    void setPositionSeconds(double seconds);

    /// Read the current play position in seconds.
    double getPlayPositionSeconds() const;

    /// Start playback.
    void play();

    /// Stop playback.
    void stop();

private:
    juce::VideoComponent component;
    juce::File currentFile;
};
