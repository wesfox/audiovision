#pragma once

#include <JuceHeader.h>

/// Import and export track clip timing metadata.
class TrackSerializer
{
public:
    /// Clip timing data in milliseconds for session I/O.
    struct ClipsWithMilliseconds
    {
        juce::File file;
        int startMs = 0;
        int endMs = 0;
        int timerefMs = 0;

        bool operator==(const juce::File& other) const { return file.getFullPathName() == other.getFullPathName(); }
    };

    /// Import clips from a session file.
    /// @param sessionFile input file
    static juce::Array<ClipsWithMilliseconds> importFromFile(const juce::File& sessionFile);

    /// Export clips to a session file.
    /// @param tracks clip timing data
    /// @param sessionFile output file
    static bool exportToFile(const juce::Array<ClipsWithMilliseconds>& tracks, const juce::File& sessionFile);
};
