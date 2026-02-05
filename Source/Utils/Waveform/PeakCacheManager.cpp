#include "PeakCacheManager.h"

#include <cmath>

PeakCacheManager& PeakCacheManager::get() {
    static PeakCacheManager instance;
    return instance;
}

std::shared_ptr<PeakFile> PeakCacheManager::getOrBuildPeakFile(const juce::File& audioFile,
                                                               juce::AudioFormatReader& reader) {
    const auto key = audioFile.getFullPathName();
    {
        const juce::ScopedLock scopedLock(lock);
        auto it = cache.find(key);
        if (it != cache.end()) {
            if (auto cached = it->second.lock()) {
                return cached;
            }
        }
    }

    const auto peakFilePath = getPeakFilePath(audioFile);
    const auto audioModified = audioFile.getLastModificationTime();
    bool shouldBuild = !peakFilePath.existsAsFile()
                       || peakFilePath.getLastModificationTime() < audioModified;

    std::shared_ptr<PeakFile> peakFile;
    if (!shouldBuild) {
        peakFile = PeakFile::open(peakFilePath);
        if (peakFile != nullptr) {
            const auto expectedSamples = static_cast<uint64>(reader.lengthInSamples);
            const auto expectedChannels = static_cast<uint32>(reader.numChannels);
            const auto expectedRate = static_cast<uint32>(std::lround(reader.sampleRate));
            if (peakFile->getTotalSamples() != expectedSamples
                || peakFile->getChannelCount() != expectedChannels
                || peakFile->getSampleRate() != expectedRate
                || peakFile->getBaseBlockSize() != baseBlockSize) {
                peakFile.reset();
                shouldBuild = true;
            }
        } else {
            shouldBuild = true;
        }
    }

    if (shouldBuild) {
        PeakFileBuilder::BuildOptions options;
        options.baseBlockSize = baseBlockSize;
        options.targetMaxBlocks = targetMaxBlocks;
        if (!builder.build(reader, peakFilePath, options)) {
            return nullptr;
        }
        peakFile = PeakFile::open(peakFilePath);
    }

    if (peakFile == nullptr) {
        return nullptr;
    }

    {
        const juce::ScopedLock scopedLock(lock);
        cache[key] = peakFile;
    }

    return peakFile;
}

void PeakCacheManager::setBaseBlockSize(uint32 newBaseBlockSize) {
    if (newBaseBlockSize == 0) {
        // Base block size must be non-zero.
        jassert(false);
        return;
    }
    baseBlockSize = newBaseBlockSize;
}

juce::File PeakCacheManager::getPeakFilePath(const juce::File& audioFile) const {
    const auto extension = audioFile.getFileExtension() + ".peak";
    return audioFile.withFileExtension(extension);
}
