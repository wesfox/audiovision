#pragma once

#include <JuceHeader.h>

#include <unordered_map>

#include "PeakFile.h"
#include "PeakFileBuilder.h"

/// Manages on-disk peak caches for audio files.
class PeakCacheManager {
public:
    /// Access the shared peak cache manager.
    static PeakCacheManager& get();

    /// Get or build a peak file for an audio reader.
    /// @param audioFile audio file path
    /// @param reader audio reader for the file
    std::shared_ptr<PeakFile> getOrBuildPeakFile(const juce::File& audioFile,
                                                 juce::AudioFormatReader& reader);

    /// Update the base block size for new peak files.
    /// @param baseBlockSize base block size in samples
    void setBaseBlockSize(uint32 baseBlockSize);

private:
    PeakCacheManager() = default;

    juce::File getPeakFilePath(const juce::File& audioFile) const;

    juce::CriticalSection lock;
    std::unordered_map<String, std::weak_ptr<PeakFile>> cache;
    PeakFileBuilder builder;
    uint32 baseBlockSize = 512;
    uint32 targetMaxBlocks = 256;
};
