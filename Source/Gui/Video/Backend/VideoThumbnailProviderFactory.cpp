#include "VideoThumbnailProviderFactory.h"

#include "VideoThumbnailProvider_AVFoundation.h"

std::unique_ptr<VideoThumbnailProvider> createVideoThumbnailProvider() {
    return std::make_unique<VideoThumbnailProvider_AVFoundation>();
}
