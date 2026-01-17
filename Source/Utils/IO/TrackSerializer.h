#pragma once

#include <JuceHeader.h>

// ------------------------ Track Serializer ------------------------

class TrackSerializer
{
public:
    struct ClipsWithMilliseconds
    {
        juce::File file;
        int startMs = 0;
        int endMs = 0;
        int timerefMs = 0;

        bool operator==(const juce::File& other) const { return file.getFullPathName() == other.getFullPathName(); }
    };

    // Import tracks from a session file
    static juce::Array<ClipsWithMilliseconds> importFromFile(const juce::File& sessionFile);

    // Export tracks to a session file
    static bool exportToFile(const juce::Array<ClipsWithMilliseconds>& tracks, const juce::File& sessionFile);
};