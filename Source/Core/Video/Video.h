#pragma once

#include <JuceHeader.h>

#include "VideoClip.h"

/// Placeholder for video metadata and sync.
class Video {
public:
    /// Create an empty video container.
    Video();
    ~Video() = default;

    /// Read the current video clips.
    const std::vector<VideoClip>& getClips() const;

    /// Add a new video clip.
    /// @param clip clip to add
    void addClip(VideoClip clip);

    /// Find the clip that contains a given frame.
    /// @param frame frame index to query
    const VideoClip* findClipForFrame(int64 frame) const;

private:
    std::vector<VideoClip> clips;
};
