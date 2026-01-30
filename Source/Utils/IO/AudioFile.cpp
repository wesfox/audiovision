#include "AudioFile.h"
#include <Utils/Uuid.h>
#include "Utils/Waveform/ThumbnailCache.h"

AudioFile::AudioFile(String filePath)
    : id(uuid::generate_uuid_v4()),
      filePath(filePath),
      originalTimeReference(0),
      channel(Channel::Mono),
      format(ChannelsFormat::Mono)
{}

/// This is the only way of creating and getting an audio file, this must be called using AudioFile::get(filePath)
/// It return an already (and shared) reference of an AudioFile if the file has already been loaded.
/// @param file AudioFile to import
std::shared_ptr<AudioFile> AudioFile::get(const juce::File& file) {
    if (!file.exists()) {
        spdlog::error("File does not exist {} ", file.getFullPathName().toStdString());
        throw;
    }

    String fullPathName = file.getFullPathName();

    static std::unordered_map<String, std::weak_ptr<AudioFile>> audioFileCache;

    if (auto existing = audioFileCache[fullPathName].lock())
        return existing;

    auto created = std::make_shared<AudioFile>(fullPathName);

    created->readWholeFileInCache();

    audioFileCache[fullPathName] = created;
    return created;
}

String AudioFile::getId() const
{
    return id;
}

String AudioFile::getFilePath() const
{
    return filePath;
}

String AudioFile::getFileName() const {
    return juce::File(filePath).getFileName();
}

ChannelsFormat AudioFile::getFormat() const
{
    return format;
}

juce::AudioBuffer<float> AudioFile::read(juce::int64 startSample, juce::int64 numberOfSamples) const {
    juce::AudioBuffer<float> buffer(ChannelCount(format), numberOfSamples);

    buffer.clear();

    reader->read(&buffer,
                 0,
                 numberOfSamples,
                 startSample,
                 true,
                 true);

    return buffer;
}

void AudioFile::readWholeFileInCache() {
    AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    AudioFormat *audioFormat = formatManager.getDefaultFormat();

    reader = std::unique_ptr<juce::MemoryMappedAudioFormatReader>(
    audioFormat->createMemoryMappedReader(juce::File(filePath))
    );
    reader->mapEntireFile();

    const auto file = juce::File(filePath);
    if (thumbnail == nullptr) {
        auto& thumbnailCache = ThumbnailCache::get();
        thumbnail = std::make_unique<juce::AudioThumbnail>(thumbnailCache.getSamplesPerThumbSample(),
                                                           thumbnailCache.getFormatManager(),
                                                           thumbnailCache.getCache());
        thumbnail->setSource(new juce::FileInputSource(file));
    }
}

juce::AudioThumbnail* AudioFile::getThumbnail() const {
    if (thumbnail == nullptr) {
        // Thumbnail must be created during file load to avoid painting without a source.
        jassert(false);
        return nullptr;
    }
    return thumbnail.get();
}
