#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"
#include "Gui/Video/VideoRenderer.h"
#include "Gui/Video/VideoThumbnailCache.h"

/// Coordinate transport-driven video sync.
class VideoSyncController {
public:
    /// Bind the controller to edit and renderers.
    VideoSyncController(Edit& edit, VideoRenderer& renderer, VideoThumbnailCache& thumbnailCache);

    /// Register a callback when the active clip changes.
    /// @param callback handler invoked with clip name
    void onActiveClipChanged(std::function<void(const juce::String&)> callback);

    /// Update playback state based on transport.
    void update();

private:
    void updatePendingInitialFrame(double frameRate);
    const VideoClip* resolveClipForFrame(int64 frame) const;

    Edit& edit;
    VideoRenderer& renderer;
    VideoThumbnailCache& thumbnailCache;
    const VideoClip* activeClip = nullptr;
    std::function<void(const juce::String&)> activeClipChanged;
    bool pendingPlay = false;
    bool pendingInitialFrame = false;
    juce::File pendingInitialFrameFile;
    int64_t lastPausedFrame = std::numeric_limits<int64_t>::min();
    bool pendingPausedFrame = false;
    bool checkedNominalFrameRate = false;
    bool wasPlaying = false;

    JUCE_DECLARE_WEAK_REFERENCEABLE(VideoSyncController)
};
