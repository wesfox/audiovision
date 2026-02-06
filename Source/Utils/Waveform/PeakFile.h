#pragma once

#include <JuceHeader.h>

#include "PeakFileFormat.h"

/// Read-only access to a waveform peak cache.
class PeakFile {
public:
    struct PeakBlock {
        int16 min = 0;
        int16 max = 0;
    };

    /// Open a peak cache file.
    /// @param peakFilePath cache file path
    static std::shared_ptr<PeakFile> open(const juce::File& peakFilePath);

    /// True when the file is ready for reading.
    bool isValid() const;

    /// Best resolution for a given samples-per-pixel ratio.
    /// @param samplesPerPixel view resolution in samples
    uint32 getBestResolution(double samplesPerPixel) const;

    /// Read blocks for a sample range at a resolution.
    /// @param samplesPerBlock resolution to read
    /// @param startSample first sample in file space
    /// @param endSample last sample in file space
    /// @param outBlocks output blocks (per channel)
    bool readBlocksForRange(uint32 samplesPerBlock,
                            uint64 startSample,
                            uint64 endSample,
                            std::vector<PeakBlock>& outBlocks) const;

    /// Number of channels in the cached audio.
    uint32 getChannelCount() const noexcept {
        return channelCount;
    }

    /// Total samples in the cached audio.®
    uint64 getTotalSamples() const noexcept {
        return totalSamples;
    }

    /// Sample rate stored in the cache.
    uint32 getSampleRate() const noexcept {
        return sampleRate;
    }

    /// Base block size for the cache.
    uint32 getBaseBlockSize() const noexcept {
        return baseBlockSize;
    }

private:
    explicit PeakFile(const juce::File& peakFilePath);

    bool load();

    juce::File peakFilePath;
    std::unique_ptr<juce::MemoryMappedFile> mappedFile;
    struct LevelInfo {
        uint64 offset = 0;
        uint32 blockSize = 0;
        uint32 blockCount = 0;
    };
    std::vector<LevelInfo> levels;
    PeakFileFormat::SampleFormat sampleFormat = PeakFileFormat::SampleFormat::Int16;
    uint32 sampleRate = 0;
    uint32 channelCount = 0;
    uint64 totalSamples = 0;
    uint32 baseBlockSize = 0;
};
