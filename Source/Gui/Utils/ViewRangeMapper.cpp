#include "ViewRangeMapper.h"

#include <algorithm>
#include <cmath>

ViewRangeMapper::ViewRangeMapper(const Edit& edit, float width)
    : viewStartSampleF(edit.getState().getViewStartSampleF()),
      viewLengthSamplesF(edit.getState().getSamplesPerPixel() * static_cast<double>(width)),
      width(width) {
    viewLength = std::max<int64>(1, static_cast<int64>(std::llround(viewLengthSamplesF)));
    viewStart = static_cast<int64>(std::llround(viewStartSampleF));
    viewEnd = viewStart + viewLength;
}

bool ViewRangeMapper::isValid() const {
    return viewLengthSamplesF > 0.0 && width > 0.0f;
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
    const auto delta = static_cast<double>(clamped) - viewStartSampleF;
    return static_cast<float>((delta / viewLengthSamplesF) * static_cast<double>(width));
}

int64 ViewRangeMapper::xToSample(float x) const {
    if (!isValid()) {
        // View range and width must be valid to map pixels.
        jassert(false);
        return viewStart;
    }
    const float clampedX = juce::jlimit(0.0f, width, x);
    const auto ratio = static_cast<double>(clampedX / width);
    const auto sample = viewStartSampleF + (ratio * viewLengthSamplesF);
    return static_cast<int64>(std::llround(sample));
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
