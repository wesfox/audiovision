#include "VideoFile.h"

VideoFile::VideoFile(const juce::File& filePath, juce::String displayName)
    : file(filePath),
      name(std::move(displayName)) {
}

const juce::File& VideoFile::getFile() const {
    return file;
}

const juce::String& VideoFile::getName() const {
    return name;
}
