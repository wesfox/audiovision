#include "VideoFrameBuffer.h"

VideoFrameBuffer::VideoFrameBuffer(size_t maxFrames)
    : maxFrames(maxFrames) {
}

void VideoFrameBuffer::clear() {
    frames.clear();
}

void VideoFrameBuffer::pushFrame(int64_t frameIndex, const juce::Image& image) {
    if (!image.isValid()) {
        return;
    }
    if (!frames.empty() && frames.back().index == frameIndex) {
        return;
    }
    frames.push_back({ frameIndex, image });
    if (frames.size() > maxFrames) {
        frames.pop_front();
    }
}

juce::Image VideoFrameBuffer::getExactFrame(int64_t frameIndex) const {
    for (auto it = frames.rbegin(); it != frames.rend(); ++it) {
        if (it->index == frameIndex) {
            return it->image;
        }
    }
    return {};
}

int64_t VideoFrameBuffer::getLastFrameIndex() const {
    return frames.empty() ? -1 : frames.back().index;
}

int64_t VideoFrameBuffer::getFirstFrameIndex() const {
    return frames.empty() ? -1 : frames.front().index;
}

juce::String VideoFrameBuffer::getIndexList() const {
    juce::String list;
    for (size_t i = 0; i < frames.size(); ++i) {
        if (i > 0) {
            list << ",";
        }
        list << frames[i].index;
    }
    return list;
}

size_t VideoFrameBuffer::size() const {
    return frames.size();
}
