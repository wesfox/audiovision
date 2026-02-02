#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"
#include "Gui/Video/VideoRenderer.h"
#include "Gui/Video/VideoThumbnailCache.h"
#include "Gui/Video/VideoSyncController.h"

/// Display the edit video synchronized with the transport.
class VideoView : public juce::Component,
                  private juce::Timer {
public:
    /// Create a view bound to the edit.
    /// @param edit edit providing transport and clips
    explicit VideoView(Edit& edit);
    ~VideoView() override;

    /// Register a callback when the active clip changes.
    /// @param callback handler invoked with clip name
    void onActiveClipChanged(std::function<void(const juce::String&)> callback);

    void resized() override;

private:
    void timerCallback() override;
    Edit& edit;
    VideoRenderer renderer;
    VideoThumbnailCache thumbnailCache;
    VideoSyncController syncController;
};
