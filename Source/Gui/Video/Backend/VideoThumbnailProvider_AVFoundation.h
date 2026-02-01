#pragma once

#include <juce_graphics/juce_graphics.h>

#include "VideoThumbnailProvider.h"

/// Generate thumbnails with AVFoundation.
class VideoThumbnailProvider_AVFoundation : public VideoThumbnailProvider {
public:
    VideoThumbnailProvider_AVFoundation();
    ~VideoThumbnailProvider_AVFoundation() override;

    void setVideoFile(const VideoFile& file) override;
    juce::Image getFrameAtSeconds(double seconds) override;
    bool isReady() const override;

private:
    void* impl = nullptr;
};
