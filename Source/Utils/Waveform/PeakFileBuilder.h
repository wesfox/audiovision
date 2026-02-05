#pragma once

#include <JuceHeader.h>

#include "PeakFileFormat.h"

/// Builds peak cache files from audio readers.
class PeakFileBuilder {
public:
    /// Hook for optional compression.
    class CompressionHook {
    public:
        virtual ~CompressionHook() = default;

        /// Compress a block of peak data.
        /// @param output destination stream
        /// @param data raw data
        /// @param sizeBytes size in bytes
        virtual bool compress(juce::OutputStream& output,
                              const void* data,
                              size_t sizeBytes) = 0;
    };

    /// Build configuration for a peak file.
    struct BuildOptions {
        uint32 baseBlockSize = 512;
        uint32 targetMaxBlocks = 256;
        PeakFileFormat::SampleFormat sampleFormat = PeakFileFormat::SampleFormat::Int16;
        std::shared_ptr<CompressionHook> compressionHook;
    };

    /// Build a peak file from an audio reader.
    /// @param reader source audio reader
    /// @param peakFilePath output .peak file
    /// @param options build configuration
    bool build(juce::AudioFormatReader& reader,
               const juce::File& peakFilePath,
               const BuildOptions& options);

    /// Build a peak file with default options.
    /// @param reader source audio reader
    /// @param peakFilePath output .peak file
    bool build(juce::AudioFormatReader& reader,
               const juce::File& peakFilePath);
};
