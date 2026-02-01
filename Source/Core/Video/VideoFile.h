#pragma once

#include <JuceHeader.h>

/// Store video file metadata.
class VideoFile {
public:
    /// Create a video file wrapper.
    /// @param filePath video file path
    /// @param displayName name shown in the UI
    VideoFile(const juce::File& filePath, juce::String displayName);

    /// Access the underlying file.
    const juce::File& getFile() const;

    /// Access the display name.
    const juce::String& getName() const;

private:
    juce::File file;
    juce::String name;
};
