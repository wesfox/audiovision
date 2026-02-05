#pragma once

#include <JuceHeader.h>

#include "PeakFileFormat.h"

/// Read-only access to a waveform peak cache.
class PeakFile {
public:
    struct Level {
        uint64 offset = 0;
        uint32 blockSize = 0;
        uint32 blockCount = 0;
    };

    struct PeakBlock {
        int16 min = 0;
        int16 max = 0;
    };

    /// Open a peak cache file.
    /// @param peakFilePath cache file path
    static std::shared_ptr<PeakFile> open(const juce::File& peakFilePath);

    /// True when the file is ready for reading.
    bool isValid() const;

    /// Best level for a given samples-per-pixel ratio.
    /// @param samplesPerPixel view resolution in samples
    const Level& getBestLevel(double samplesPerPixel) const;

    /// Read blocks from a level.
    /// @param level level to read from
    /// @param startBlock first block index
    /// @param blockCount number of blocks to read
    /// @param outBlocks output blocks (per channel)
    bool readBlocks(const Level& level,
                    uint64 startBlock,
                    uint32 blockCount,
                    std::vector<PeakBlock>& outBlocks) const;

    /// Number of channels in the cached audio.
    uint32 getChannelCount() const noexcept {
        return channelCount;
    }

    /// Total samples in the cached audio.
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
    std::vector<Level> levels;
    PeakFileFormat::SampleFormat sampleFormat = PeakFileFormat::SampleFormat::Int16;
    uint32 sampleRate = 0;
    uint32 channelCount = 0;
    uint64 totalSamples = 0;
    uint32 baseBlockSize = 0;
};
