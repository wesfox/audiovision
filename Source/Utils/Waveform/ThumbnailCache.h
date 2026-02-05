#pragma once

#include <JuceHeader.h>

/// Owns shared JUCE thumbnail resources, for waveform caching.
class ThumbnailCache {
public:
    /// Access the shared thumbnail cache singleton.
    static ThumbnailCache& get();

    /// Access the shared format manager, for creating readers.
    juce::AudioFormatManager& getFormatManager();

    /// Access the shared thumbnail cache.
    juce::AudioThumbnailCache& getCache();

    /// Access the configured thumbnail resolution.
    int getSamplesPerThumbSample() const;

private:
    ThumbnailCache();

    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache cache;
    int samplesPerThumbSample = 10;
};
