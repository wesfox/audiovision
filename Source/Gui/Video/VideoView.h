#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"
#include "Gui/Video/VideoRenderer.h"
#include "Gui/Video/VideoThumbnailCache.h"

/// Display the edit video synchronized with the transport.
class VideoView : public juce::Component,
                  private juce::Timer {
public:
    /// Create a view bound to the edit.
    /// @param edit edit providing transport and clips
    explicit VideoView(Edit& edit);

    /// Register a callback when the active clip changes.
    /// @param callback handler invoked with clip name
    void onActiveClipChanged(std::function<void(const juce::String&)> callback);

    void resized() override;

private:
    void timerCallback() override;
    void updateFromTransport();
    void updatePendingInitialFrame(double frameRate);
    const VideoClip* resolveClipForFrame(int64 frame) const;

    Edit& edit;
    VideoRenderer renderer;
    VideoThumbnailCache thumbnailCache;
    const VideoClip* activeClip = nullptr;
    std::function<void(const juce::String&)> activeClipChanged;
    bool wasPlaying = false;
    bool pendingPlay = false;
    bool pendingInitialFrame = false;
    bool pendingInitialFrameRequested = false;
    juce::File pendingInitialFrameFile;
    int64_t lastPausedFrame = std::numeric_limits<int64_t>::min();
    bool pendingPausedFrame = false;
};
