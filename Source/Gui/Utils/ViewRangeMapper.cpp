#include "ViewRangeMapper.h"

ViewRangeMapper::ViewRangeMapper(const Edit& edit, float width)
    : viewStart(edit.getViewStartSample()),
      viewEnd(edit.getViewEndSample()),
      viewLength(viewEnd - viewStart),
      width(width) {
}

bool ViewRangeMapper::isValid() const {
    return viewLength > 0 && width > 0.0f;
}

ViewRangeMapper ViewRangeMapper::createChecked(const Edit& edit, float width) {
    ViewRangeMapper mapper(edit, width);
    if (!mapper.isValid()) {
        // View range and width must be valid for mapping.
        jassert(false);
    }
    return mapper;
}

int64 ViewRangeMapper::clampSample(int64 sample) const {
    if (!isValid()) {
        // View range and width must be valid to clamp samples.
        jassert(false);
        return viewStart;
    }
    return std::clamp(sample, viewStart, viewEnd);
}

std::pair<int64, int64> ViewRangeMapper::clampRangeToView(int64 start, int64 end) const {
    if (!isValid()) {
        // View range and width must be valid to clamp sample ranges.
        jassert(false);
        return { viewStart, viewStart };
    }
    const auto clampedStart = std::clamp(start, viewStart, viewEnd);
    const auto clampedEnd = std::clamp(end, viewStart, viewEnd);
    return { clampedStart, clampedEnd };
}

float ViewRangeMapper::sampleToX(int64 sample) const {
    if (!isValid()) {
        // View range and width must be valid to map samples.
        jassert(false);
        return 0.0f;
    }
    const auto clamped = clampSample(sample);
    return (static_cast<float>(clamped - viewStart) / static_cast<float>(viewLength)) * width;
}

int64 ViewRangeMapper::xToSample(float x) const {
    if (!isValid()) {
        // View range and width must be valid to map pixels.
        jassert(false);
        return viewStart;
    }
    const float clampedX = juce::jlimit(0.0f, width, x);
    return viewStart
        + static_cast<int64>(std::llround((clampedX / width) * static_cast<float>(viewLength)));
}

int64 ViewRangeMapper::getViewStartSample() const {
    return viewStart;
}

int64 ViewRangeMapper::getViewEndSample() const {
    return viewEnd;
}

int64 ViewRangeMapper::getViewLengthSamples() const {
    return viewLength;
}
