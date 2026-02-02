#pragma once

#include <juce_graphics/juce_graphics.h>

#include <deque>

/// Store decoded video frames by index.
class VideoFrameBuffer {
public:
    /// @param maxFrames maximum stored frames
    explicit VideoFrameBuffer(size_t maxFrames);

    /// Clear all buffered frames.
    void clear();

    /// Push a new frame into the buffer.
    /// @param frameIndex decoded frame index
    /// @param image decoded frame image
    void pushFrame(int64_t frameIndex, const juce::Image& image);

    /// Fetch an exact frame if present.
    /// @param frameIndex frame index to find
    juce::Image getExactFrame(int64_t frameIndex) const;

    /// Read the last frame index in the buffer.
    int64_t getLastFrameIndex() const;

    /// Read the first frame index in the buffer.
    int64_t getFirstFrameIndex() const;

    /// Build a comma-separated list of buffered frame indices for diagnostics.
    juce::String getIndexList() const;

    /// Read the number of buffered frames.
    size_t size() const;

private:
    struct Entry {
        int64_t index = -1;
        juce::Image image;
    };

    size_t maxFrames = 0;
    std::deque<Entry> frames;
};
