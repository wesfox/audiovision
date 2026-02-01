#include "VideoView.h"

VideoView::VideoView(Edit& edit)
    : edit(edit) {
    addAndMakeVisible(renderer.getComponent());
    startTimerHz(60);
}

void VideoView::onActiveClipChanged(std::function<void(const juce::String&)> callback) {
    activeClipChanged = std::move(callback);
}

void VideoView::resized() {
    renderer.getComponent().setBounds(getLocalBounds());
}

void VideoView::timerCallback() {
    updateFromTransport();
}

void VideoView::updateFromTransport() {
    const auto transport = edit.getTransport();
    if (!transport) {
        return;
    }
    const auto frameRate = edit.getFrameRate();
    if (frameRate <= 0.0f) {
        // Frame rate must be positive to sync video.
        jassert(false);
        return;
    }
    const auto frame = transport->getCurrentFrame();
    const auto clip = resolveClipForFrame(frame);
    if (!clip) {
        return;
    }
    if (clip != activeClip) {
        activeClip = clip;
        renderer.setVideoFile(*activeClip->getFile());
        if (activeClipChanged) {
            activeClipChanged(activeClip->getName());
        }
    }
    const double frameOffset = static_cast<double>(frame - activeClip->getInFrame());
    const double roundedFrame = std::ceil(frameOffset);
    const double seconds = (roundedFrame + 1) / frameRate;
    const bool isPlaying = transport->isPlaying();
    if (isPlaying && !wasPlaying) {
        renderer.setPositionSeconds(seconds);
        lastResyncMs = juce::Time::getMillisecondCounter() + 1000.0;
    }
    if (isPlaying) {
        if (!renderer.getComponent().isPlaying()) {
            renderer.play();
        }
        const double drift = std::abs(renderer.getPlayPositionSeconds() - seconds);
        const double driftThreshold = 2.0 / frameRate;
        const auto nowMs = juce::Time::getMillisecondCounter();
        juce::Logger::writeToLog("Video frame drift : " + juce::String(drift * frameRate)
            + " - Threshold : " + juce::String(driftThreshold * frameRate) + " - Frame rate : " + juce::String(frameRate));
        if (drift > driftThreshold && (nowMs - lastResyncMs) >= 3000) {
            juce::Logger::writeToLog("Drift Corrected");
            renderer.setPositionSeconds(seconds);
            lastResyncMs = nowMs;
        }
    } else {
        renderer.setPositionSeconds(seconds);
        renderer.stop();
        lastResyncMs = 0;
    }
    wasPlaying = isPlaying;
}

const VideoClip* VideoView::resolveClipForFrame(int64 frame) const {
    return edit.getVideo().findClipForFrame(frame);
}
