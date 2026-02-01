#include "VideoRenderer.h"

VideoRenderer::VideoRenderer()
    : component(false) {
}

juce::VideoComponent& VideoRenderer::getComponent() {
    return component;
}

void VideoRenderer::setVideoFile(const VideoFile& file) {
    const auto nextFile = file.getFile();
    if (currentFile == nextFile) {
        return;
    }
    currentFile = nextFile;
    component.load(currentFile);
}

void VideoRenderer::setPositionSeconds(double seconds) {
    component.setPlayPosition(seconds);
}

double VideoRenderer::getPlayPositionSeconds() const {
    return component.getPlayPosition();
}

void VideoRenderer::play() {
    component.play();
}

void VideoRenderer::stop() {
    component.stop();
}
