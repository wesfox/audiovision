#pragma once

#include <JuceHeader.h>
#include <Utils/Format.h>
#include <Utils/Channel.h>
#include <vector>
#include <spdlog/spdlog.h>

#include "juce_graphics/fonts/harfbuzz/hb-cplusplus.hh"

/// Allows to read with low latency from a file
/// TODO : Handle multi-mono files
class AudioFile {
public:
    explicit AudioFile(String filePath);
    ~AudioFile() = default;

    static std::shared_ptr<AudioFile> get (const juce::File& file);

    // Getters
    String getId() const;
    String getFilePath() const;
    String getFileName() const;

    /// Channel format used by the file, can be inferred by metadata or filenames used for the import
    ChannelsFormat getFormat() const;

    /// Used to read in an audio file, juce::MemoryMappedAudioFormatReader is used to get low latency access
    /// Parameters Use[ReaderLeft/RightChannels] are set to false because it seems that they force, in stereo,
    /// a copy from the left channel to the right channel or the opposite.
    /// @param startSample sample position (in the whole file, 0 being the start), at which we want to start reading
    /// @param numberOfSamples number of sample to be read
    /// @see juce::MemoryMappedAudioFormatReader
    juce::AudioBuffer<float> read(juce::int64 startSample, juce::int64 numberOfSamples) const;

    /// Used to read the whole file in order to be able to access it with low latency (RAM access)
    /// Called at initialization, another call is only needed if the file content has changed.
    /// Todo : Make optimized reader that allow to read only parts of the files that are used.
    void readWholeFileInCache();
private:

    String id;
    String filePath;
    std::unique_ptr<juce::MemoryMappedAudioFormatReader> reader;
    std::vector<float> waveformData;
    int64 originalTimeReference;
    Channel channel;
    ChannelsFormat format;
};
