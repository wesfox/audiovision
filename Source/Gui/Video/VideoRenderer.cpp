#include "VideoRenderer.h"

#include "Gui/Video/Backend/VideoBackendFactory.h"

VideoRenderer::VideoRenderer()
    : backend(createVideoBackend()) {
}

void VideoRenderer::setVideoFile(const VideoFile& file) {
    const auto nextFile = file.getFile();
    if (currentFile == nextFile) {
        return;
    }
    currentFile = nextFile;
    if (backend) {
        backend->loadFile(currentFile);
    }
}

void VideoRenderer::setPositionSeconds(double seconds) {
    if (backend) {
        backend->setPlayheadSeconds(seconds);
        refreshFrame();
    }
}

void VideoRenderer::setPlayheadSeconds(double seconds) {
    if (backend) {
        backend->setPlayheadSeconds(seconds);
    }
}

void VideoRenderer::setFrameRate(double frameRate) {
    if (backend) {
        backend->setFrameRate(frameRate);
    }
}

double VideoRenderer::getPlayPositionSeconds() const {
    return backend ? backend->getPlayheadSeconds() : 0.0;
}

void VideoRenderer::play() {
    if (backend) {
        backend->play();
    }
}

void VideoRenderer::stop() {
    if (backend) {
        backend->stop();
    }
}

bool VideoRenderer::isReady() const {
    return backend && backend->isReady();
}

bool VideoRenderer::isPlaying() const {
    return backend && backend->isPlaying();
}

int64_t VideoRenderer::getLastFrameIndex() const {
    return backend ? backend->getLastFrameIndex() : -1;
}

double VideoRenderer::getNominalFrameRate() const {
    return backend ? backend->getNominalFrameRate() : 0.0;
}

void VideoRenderer::setPreviewFrame(const juce::Image& frame) {
    if (!frame.isValid()) {
        return;
    }
    currentFrame = frame;
    repaint();
}

void VideoRenderer::refreshFrame() {
    if (backend) {
        auto frame = backend->getCurrentFrameImage();
        if (!frame.isValid()) {
            return;
        }
        currentFrame = std::move(frame);
        repaint();
    }
}

void VideoRenderer::paint(juce::Graphics& g) {
    if (!currentFrame.isValid()) {
        g.fillAll(juce::Colours::black);
        return;
    }
    g.drawImage(currentFrame, getLocalBounds().toFloat());
}
