#include "VideoClip.h"

VideoClip::VideoClip(std::shared_ptr<VideoFile> file, int64 inFrame, int64 outFrame)
    : file(std::move(file)),
      inFrame(inFrame),
      outFrame(outFrame) {
    if (this->file != nullptr) {
        name = this->file->getName();
    }
}

const std::shared_ptr<VideoFile>& VideoClip::getFile() const {
    return file;
}

const juce::String& VideoClip::getName() const {
    return name;
}

int64 VideoClip::getInFrame() const {
    return inFrame;
}

int64 VideoClip::getOutFrame() const {
    return outFrame;
}

bool VideoClip::containsFrame(int64 frame) const {
    if (frame < inFrame) {
        return false;
    }
    if (outFrame < 0) {
        return true;
    }
    return frame <= outFrame;
}
