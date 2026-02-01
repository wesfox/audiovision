#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"

/// Map between timeline samples and pixel positions.
class ViewRangeMapper {
public:
    /// Create a mapper for the current view and width.
    /// @param edit edit providing the view range
    /// @param width pixel width of the view
    ViewRangeMapper(const Edit& edit, float width);

    /// Check whether the view range and width are valid.
    bool isValid() const;

    /// Create a mapper and assert when invalid.
    /// @param edit edit providing the view range
    /// @param width pixel width of the view
    static ViewRangeMapper createChecked(const Edit& edit, float width);

    /// Clamp a sample to the visible view range.
    /// @param sample sample to clamp
    int64 clampSample(int64 sample) const;

    /// Clamp a sample range to the visible view range.
    /// @param start range start sample
    /// @param end range end sample
    std::pair<int64, int64> clampRangeToView(int64 start, int64 end) const;

    /// Convert a sample position to a pixel X value.
    /// @param sample sample to map
    float sampleToX(int64 sample) const;

    /// Convert a pixel X value to a sample position.
    /// @param x pixel to map
    int64 xToSample(float x) const;

    /// Access the view start sample.
    int64 getViewStartSample() const;

    /// Access the view end sample.
    int64 getViewEndSample() const;

    /// Access the view length in samples.
    int64 getViewLengthSamples() const;

private:
    int64 viewStart = 0;
    int64 viewEnd = 0;
    int64 viewLength = 0;
    float width = 0.0f;
};
