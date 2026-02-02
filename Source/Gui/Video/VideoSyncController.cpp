#include "VideoSyncController.h"

#include <cmath>

VideoSyncController::VideoSyncController(Edit& edit,
                                         VideoRenderer& renderer,
                                         VideoThumbnailCache& thumbnailCache)
    : edit(edit),
      renderer(renderer),
      thumbnailCache(thumbnailCache) {
}

void VideoSyncController::onActiveClipChanged(std::function<void(const juce::String&)> callback) {
    activeClipChanged = std::move(callback);
}

void VideoSyncController::update() {
    const auto transport = edit.getTransport();
    if (!transport) {
        // Transport must exist to drive video sync.
        jassert(false);
        return;
    }
    const auto frameRate = edit.getFrameRate();
    if (frameRate <= 0.0f) {
        // Frame rate must be positive to sync video.
        jassert(false);
        return;
    }
    renderer.setFrameRate(frameRate);
    const auto frame = transport->getCurrentFrame();
    const auto clip = resolveClipForFrame(frame);
    if (!clip) {
        return;
    }
    if (clip != activeClip) {
        activeClip = clip;
        renderer.setVideoFile(*activeClip->getFile());
        thumbnailCache.setVideoFile(*activeClip->getFile());
        pendingInitialFrame = true;
        pendingInitialFrameFile = activeClip->getFile()->getFile();
        checkedNominalFrameRate = false;
        if (activeClipChanged) {
            activeClipChanged(activeClip->getName());
        }
    }
    if (!renderer.isReady()) {
        return;
    }
    if (!checkedNominalFrameRate) {
        const auto nominalFrameRate = renderer.getNominalFrameRate();
        if (nominalFrameRate > 0.0 && std::abs(nominalFrameRate - frameRate) > 0.01) {
            // Video frame rate must match the edit frame rate.
            jassert(false);
        }
        checkedNominalFrameRate = true;
    }
    if (pendingInitialFrame) {
        updatePendingInitialFrame(frameRate);
    }
    if (pendingPlay && renderer.isReady()) {
        transport->play();
        pendingPlay = false;
    }
    const double frameOffset = static_cast<double>(frame - activeClip->getInFrame());
    const double roundedFrame = std::round(frameOffset);
    const double seconds = roundedFrame / frameRate;
    const bool isPlaying = transport->isPlaying();
    if (isPlaying && !wasPlaying) {
        thumbnailCache.cancelPending();
        pendingPausedFrame = false;
        lastPausedFrame = std::numeric_limits<int64_t>::min();
    } else if (!isPlaying && wasPlaying) {
        lastPausedFrame = std::numeric_limits<int64_t>::min();
    }
    if (isPlaying && !renderer.isReady()) {
        pendingPlay = true;
        transport->stop();
        return;
    }
    if (isPlaying) {
        if (!renderer.isPlaying()) {
            renderer.setPositionSeconds(seconds);
            renderer.play();
        }
        renderer.setPlayheadSeconds(seconds);
        renderer.refreshFrame();
        const auto backendFrame = renderer.getLastFrameIndex();
        if (backendFrame >= 0) {
            const auto targetFrame = static_cast<int64_t>(std::llround(seconds * frameRate));
            const auto frameDiff = std::llabs(backendFrame - targetFrame);
            juce::ignoreUnused(frameDiff);
        }
    } else {
        renderer.stop();
        if (pendingInitialFrame) {
            return;
        }
        if (frame != lastPausedFrame && !pendingPausedFrame) {
            const auto expectedFile = pendingInitialFrameFile;
            const auto expectedFrame = frame;
            const auto clipFrame = frame - activeClip->getInFrame();
            pendingPausedFrame = true;
            auto weakSelf = juce::WeakReference<VideoSyncController>(this);
            thumbnailCache.requestFrameIndex(clipFrame, frameRate,
                [weakSelf, expectedFile, expectedFrame](const juce::Image& frameImage) {
                    if (auto* self = weakSelf.get()) {
                        if (!frameImage.isValid()) {
                            self->pendingPausedFrame = false;
                            return;
                        }
                        if (self->activeClip == nullptr
                            || self->activeClip->getFile()->getFile() != expectedFile
                            || self->pendingInitialFrame) {
                            self->pendingPausedFrame = false;
                            return;
                        }
                        self->renderer.setPreviewFrame(frameImage);
                        self->lastPausedFrame = expectedFrame;
                        self->pendingPausedFrame = false;
                    }
                });
        }
    }
    wasPlaying = isPlaying;
}

void VideoSyncController::updatePendingInitialFrame(double frameRate) {
    if (!thumbnailCache.isReady()) {
        return;
    }
    const auto expectedFile = pendingInitialFrameFile;
    pendingInitialFrame = false;
    auto weakSelf = juce::WeakReference<VideoSyncController>(this);
    thumbnailCache.requestFrameIndex(activeClip->getInFrame(), frameRate,
        [weakSelf, expectedFile](const juce::Image& frame) {
            if (auto* self = weakSelf.get()) {
                if (!frame.isValid()) {
                    self->pendingInitialFrame = true;
                    return;
                }
                if (self->activeClip == nullptr
                    || self->activeClip->getFile()->getFile() != expectedFile) {
                    return;
                }
                self->renderer.setPreviewFrame(frame);
            }
        });
}

const VideoClip* VideoSyncController::resolveClipForFrame(int64 frame) const {
    return edit.getVideo().findClipForFrame(frame);
}
