#include "VideoView.h"

VideoView::VideoView(Edit& edit)
    : edit(edit) {
    addAndMakeVisible(renderer);
    startTimerHz(60);
}

void VideoView::onActiveClipChanged(std::function<void(const juce::String&)> callback) {
    activeClipChanged = std::move(callback);
}

void VideoView::resized() {
    renderer.setBounds(getLocalBounds());
}

void VideoView::timerCallback() {
    updateFromTransport();
}

void VideoView::updateFromTransport() {
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
        pendingInitialFrameRequested = false;
        pendingInitialFrameFile = activeClip->getFile()->getFile();
        if (activeClipChanged) {
            activeClipChanged(activeClip->getName());
        }
    }
    if (!renderer.isReady()) {
        return;
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
    if (isPlaying && !renderer.isReady()) {
        pendingPlay = true;
        transport->stop();
        return;
    }
    if (isPlaying && !wasPlaying) {
        renderer.setPositionSeconds(seconds);
    }
    if (isPlaying) {
        if (!renderer.isPlaying()) {
            renderer.play();
        }
        renderer.setPositionSeconds(seconds);
        const auto backendFrame = renderer.getLastFrameIndex();
        if (backendFrame >= 0) {
            const auto targetFrame = static_cast<int64_t>(std::llround(seconds * frameRate));
            const auto frameDiff = std::llabs(backendFrame - targetFrame);
            juce::Logger::writeToLog("Video frame rendered: " + juce::String(backendFrame)
                + " - Drift: " + juce::String(frameDiff));
        }
    } else {
        renderer.stop();
        if (pendingInitialFrame) {
            return;
        }
        if (frame != lastPausedFrame && !pendingPausedFrame) {
            const auto expectedFile = pendingInitialFrameFile;
            const auto expectedFrame = frame;
            pendingPausedFrame = true;
            thumbnailCache.requestFrameSeconds(seconds, [this, expectedFile, expectedFrame](const juce::Image& frameImage) {
                if (!frameImage.isValid()) {
                    pendingPausedFrame = false;
                    return;
                }
                if (activeClip == nullptr
                    || activeClip->getFile()->getFile() != expectedFile
                    || edit.getTransport() == nullptr
                    || edit.getTransport()->isPlaying()
                    || edit.getTransport()->getCurrentFrame() != expectedFrame) {
                    pendingPausedFrame = false;
                    return;
                }
                renderer.setPreviewFrame(frameImage);
                lastPausedFrame = expectedFrame;
                pendingPausedFrame = false;
            });
        }
    }
    wasPlaying = isPlaying;
}

void VideoView::updatePendingInitialFrame(double frameRate) {
    if (pendingInitialFrameRequested) {
        return;
    }
    if (!thumbnailCache.isReady()) {
        return;
    }
    const double inSeconds = static_cast<double>(activeClip->getInFrame()) / frameRate;
    const auto expectedFile = pendingInitialFrameFile;
    pendingInitialFrameRequested = true;
    thumbnailCache.requestFrameSeconds(inSeconds, [this, expectedFile](const juce::Image& frame) {
        juce::Logger::writeToLog("VideoView: thumbnail callback, valid: "
            + juce::String(static_cast<int>(frame.isValid())));
        if (!frame.isValid()) {
            pendingInitialFrameRequested = false;
            return;
        }
        if (!pendingInitialFrame || activeClip == nullptr
            || activeClip->getFile()->getFile() != expectedFile) {
            juce::Logger::writeToLog("VideoView: thumbnail callback ignored (state changed).");
            pendingInitialFrameRequested = false;
            return;
        }
        juce::Logger::writeToLog("VideoView: applying thumbnail preview.");
        renderer.setPreviewFrame(frame);
        pendingInitialFrame = false;
        pendingInitialFrameRequested = false;
    });
}

const VideoClip* VideoView::resolveClipForFrame(int64 frame) const {
    return edit.getVideo().findClipForFrame(frame);
}
