#pragma once

#include <memory>

#include "VideoBackend.h"

/// Create the platform video backend.
std::unique_ptr<VideoBackend> createVideoBackend();
