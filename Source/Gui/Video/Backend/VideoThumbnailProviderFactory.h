#pragma once

#include <memory>

#include "VideoThumbnailProvider.h"

/// Create a platform thumbnail provider.
std::unique_ptr<VideoThumbnailProvider> createVideoThumbnailProvider();
