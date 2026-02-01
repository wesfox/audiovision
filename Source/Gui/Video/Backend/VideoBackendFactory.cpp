#include "VideoBackendFactory.h"

#include "VideoBackend_AVFoundation.h"

std::unique_ptr<VideoBackend> createVideoBackend() {
    return std::make_unique<VideoBackend_AVFoundation>();
}
