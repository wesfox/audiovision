#include "VideoBackendFactory.h"

#include "VideoBackend_AVAssetReader.h"

std::unique_ptr<VideoBackend> createVideoBackend() {
    return std::make_unique<VideoBackend_AVAssetReader>();
}
