#include "TrackHorizontalScrollBar.h"

#include <algorithm>

namespace {
constexpr int kScrollBarHeight = 6;
constexpr float kMinThumbSize = 16.0f;
}

TrackHorizontalScrollBar::TrackHorizontalScrollBar(Edit& edit)
    : edit(edit) {
    edit.getState().getRoot().addListener(this);
}

TrackHorizontalScrollBar::~TrackHorizontalScrollBar() {
    edit.getState().getRoot().removeListener(this);
}

int TrackHorizontalScrollBar::getPreferredHeight() const {
    return kScrollBarHeight;
}

void TrackHorizontalScrollBar::paint(juce::Graphics& g) {
    const auto width = static_cast<float>(getWidth());
    const auto height = static_cast<float>(getHeight());
    const auto viewStart = edit.getViewStartSample();
    const auto viewEnd = edit.getViewEndSample();
    const auto viewLength = viewEnd - viewStart;
    if (width <= 0.0f || height <= 0.0f || viewLength <= 0) {
        return;
    }

    const auto totalSamples = std::max<int64>(getTotalSamples(), viewEnd);
    if (totalSamples <= 0) {
        return;
    }

    g.setColour(juce::Colour::fromString("#332F45"));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 3.0f);

    float thumbSize = width * (static_cast<float>(viewLength) / static_cast<float>(totalSamples));
    thumbSize = std::clamp(thumbSize, kMinThumbSize, width);
    float thumbStart = 0.0f;
    if (totalSamples > viewLength) {
        const auto maxStart = width - thumbSize;
        const auto ratio = static_cast<float>(viewStart)
            / static_cast<float>(totalSamples - viewLength);
        thumbStart = std::clamp(ratio * maxStart, 0.0f, maxStart);
    }
    lastThumbStart = thumbStart;

    g.setColour(juce::Colour::fromString("#FF998892"));
    g.fillRoundedRectangle(thumbStart, 0.0f, thumbSize, height, 3.0f);
}

void TrackHorizontalScrollBar::mouseDown(const juce::MouseEvent& event) {
    const auto width = static_cast<float>(getWidth());
    const auto viewStart = edit.getViewStartSample();
    const auto viewEnd = edit.getViewEndSample();
    const auto viewLength = viewEnd - viewStart;
    const auto totalSamples = std::max<int64>(getTotalSamples(), viewEnd);
    if (width <= 0.0f || viewLength <= 0 || totalSamples <= 0) {
        return;
    }

    float thumbSize = width * (static_cast<float>(viewLength) / static_cast<float>(totalSamples));
    thumbSize = std::clamp(thumbSize, kMinThumbSize, width);
    const float mouseX = event.position.x;
    const float thumbEnd = lastThumbStart + thumbSize;
    if (mouseX >= lastThumbStart && mouseX <= thumbEnd) {
        isDragging = true;
        dragOffset = mouseX - lastThumbStart;
        return;
    }

    isDragging = true;
    dragOffset = thumbSize * 0.5f;
    setViewStartFromThumb(mouseX - dragOffset);
}

void TrackHorizontalScrollBar::mouseDrag(const juce::MouseEvent& event) {
    if (!isDragging) {
        return;
    }
    setViewStartFromThumb(event.position.x - dragOffset);
}

void TrackHorizontalScrollBar::mouseUp(const juce::MouseEvent&) {
    isDragging = false;
}

void TrackHorizontalScrollBar::valueTreePropertyChanged(juce::ValueTree&,
                                                        const juce::Identifier&) {
    repaint();
}

int64 TrackHorizontalScrollBar::getTotalSamples() const {
    int64 totalSamples = edit.getViewEndSample();
    for (const auto& scene : edit.getScenes()) {
        if (!scene) {
            continue;
        }
        totalSamples = std::max<int64>(totalSamples, scene->sceneEndSample);
    }
    return totalSamples;
}

void TrackHorizontalScrollBar::setViewStartFromThumb(float thumbStart) {
    const auto width = static_cast<float>(getWidth());
    const auto viewStart = edit.getViewStartSample();
    const auto viewEnd = edit.getViewEndSample();
    const auto viewLength = viewEnd - viewStart;
    const auto totalSamples = std::max<int64>(getTotalSamples(), viewEnd);
    if (width <= 0.0f || viewLength <= 0 || totalSamples <= 0) {
        return;
    }

    float thumbSize = width * (static_cast<float>(viewLength) / static_cast<float>(totalSamples));
    thumbSize = std::clamp(thumbSize, kMinThumbSize, width);
    const float maxThumbStart = width - thumbSize;
    const float clampedStart = std::clamp(thumbStart, 0.0f, maxThumbStart);
    const float ratio = maxThumbStart > 0.0f ? (clampedStart / maxThumbStart) : 0.0f;
    const auto maxStartSample = std::max<int64>(0, totalSamples - viewLength);
    const auto newStart = static_cast<int64>(std::llround(ratio * static_cast<float>(maxStartSample)));
    edit.getState().setViewRange(newStart, newStart + viewLength);
}
