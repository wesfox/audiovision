#pragma once

#include <JuceHeader.h>

#include "VideoFile.h"

/// Bind a video file to a timeline range.
class VideoClip {
public:
    /// Create a clip referencing a video file.
    /// @param file video file to reference
    /// @param inFrame first frame on the timeline
    /// @param outFrame last frame on the timeline, or -1 for no end
    VideoClip(std::shared_ptr<VideoFile> file, int64 inFrame, int64 outFrame);

    /// Read the video file reference.
    const std::shared_ptr<VideoFile>& getFile() const;

    /// Access the clip display name.
    const juce::String& getName() const;

    /// Read the clip start frame.
    int64 getInFrame() const;

    /// Read the clip end frame.
    int64 getOutFrame() const;

    /// Check whether a frame falls in the clip range.
    /// @param frame frame index to test
    bool containsFrame(int64 frame) const;

private:
    std::shared_ptr<VideoFile> file;
    int64 inFrame = 0;
    int64 outFrame = -1;
    juce::String name;
};
