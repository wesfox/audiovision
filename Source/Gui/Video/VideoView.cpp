#include "VideoView.h"

VideoView::VideoView(Edit& edit)
    : edit(edit),
      syncController(edit, renderer, thumbnailCache) {
    addAndMakeVisible(renderer);
    startTimerHz(60);
}

VideoView::~VideoView() {
    thumbnailCache.cancelPending();
}

void VideoView::onActiveClipChanged(std::function<void(const juce::String&)> callback) {
    syncController.onActiveClipChanged(std::move(callback));
}

void VideoView::resized() {
    renderer.setBounds(getLocalBounds());
}

void VideoView::timerCallback() {
    syncController.update();
}
