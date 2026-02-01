#pragma once

#include <juce_graphics/juce_graphics.h>

#include "Core/Video/VideoFile.h"

/// Provide still frames from a video file.
class VideoThumbnailProvider {
public:
    virtual ~VideoThumbnailProvider() = default;

    /// Load a video file for thumbnail extraction.
    /// @param file video file to read
    virtual void setVideoFile(const VideoFile& file) = 0;

    /// Read a still frame at the requested time.
    /// @param seconds time in seconds
    virtual juce::Image getFrameAtSeconds(double seconds) = 0;

    /// Check whether the provider is ready.
    virtual bool isReady() const = 0;
};
