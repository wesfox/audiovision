#include "PeakFile.h"

#include <algorithm>
#include <cstring>

namespace {
constexpr size_t kMinimumHeaderSize = PeakFileFormat::kHeaderSize;

bool readHeader(const uint8* data, size_t size, PeakFileFormat::Header& header) {
    size_t offset = 0;
    if (!PeakFileFormat::readUint32LE(data, size, offset, header.magic)) {
        return false;
    }
    if (!PeakFileFormat::readUint16LE(data, size, offset, header.version)) {
        return false;
    }
    if (!PeakFileFormat::readUint16LE(data, size, offset, header.headerSize)) {
        return false;
    }
    if (!PeakFileFormat::readUint32LE(data, size, offset, header.sampleRate)) {
        return false;
    }
    if (!PeakFileFormat::readUint32LE(data, size, offset, header.channelCount)) {
        return false;
    }
    if (!PeakFileFormat::readUint64LE(data, size, offset, header.totalSamples)) {
        return false;
    }
    if (!PeakFileFormat::readUint32LE(data, size, offset, header.baseBlockSize)) {
        return false;
    }
    if (!PeakFileFormat::readUint32LE(data, size, offset, header.levelCount)) {
        return false;
    }
    if (offset + 1 > size) {
        return false;
    }
    header.sampleFormat = static_cast<PeakFileFormat::SampleFormat>(data[offset]);
    offset += 1;
    if (offset + 1 > size) {
        return false;
    }
    header.compression = static_cast<PeakFileFormat::Compression>(data[offset]);
    offset += 1;
    if (offset + 1 > size) {
        return false;
    }
    header.deltaEncoding = static_cast<PeakFileFormat::DeltaEncoding>(data[offset]);
    offset += 1;
    if (offset + sizeof(header.reserved) > size) {
        return false;
    }
    std::memcpy(header.reserved, data + offset, sizeof(header.reserved));
    offset += sizeof(header.reserved);
    if (!PeakFileFormat::readUint64LE(data, size, offset, header.levelTableOffset)) {
        return false;
    }
    return true;
}

bool readLevelInfo(const uint8* data, size_t size, size_t& offset, PeakFileFormat::LevelInfo& info) {
    if (!PeakFileFormat::readUint64LE(data, size, offset, info.offset)) {
        return false;
    }
    if (!PeakFileFormat::readUint32LE(data, size, offset, info.blockSize)) {
        return false;
    }
    if (!PeakFileFormat::readUint32LE(data, size, offset, info.blockCount)) {
        return false;
    }
    return true;
}

int16 readInt16LE(const uint8* data) {
    int16 value = 0;
    std::memcpy(&value, data, sizeof(int16));
    return static_cast<int16>(juce::ByteOrder::swapIfBigEndian(static_cast<uint16>(value)));
}
} // namespace

std::shared_ptr<PeakFile> PeakFile::open(const juce::File& peakFilePath) {
    auto peakFile = std::shared_ptr<PeakFile>(new PeakFile(peakFilePath));
    if (!peakFile->load()) {
        return nullptr;
    }
    return peakFile;
}

PeakFile::PeakFile(const juce::File& peakFilePath)
    : peakFilePath(peakFilePath) {
}

bool PeakFile::load() {
    if (!peakFilePath.existsAsFile()) {
        return false;
    }

    mappedFile = std::make_unique<juce::MemoryMappedFile>(peakFilePath,
                                                          juce::MemoryMappedFile::readOnly);
    if (mappedFile == nullptr || mappedFile->getData() == nullptr) {
        return false;
    }

    const auto* rawData = static_cast<const uint8*>(mappedFile->getData());
    const auto fileSize = mappedFile->getSize();
    if (fileSize < static_cast<int64>(kMinimumHeaderSize)) {
        return false;
    }

    PeakFileFormat::Header header;
    if (!readHeader(rawData, static_cast<size_t>(fileSize), header)) {
        return false;
    }
    if (header.magic != PeakFileFormat::kMagic || header.version != PeakFileFormat::kVersion) {
        return false;
    }
    if (header.headerSize != PeakFileFormat::kHeaderSize) {
        return false;
    }
    if (header.compression != PeakFileFormat::Compression::None
        || header.deltaEncoding != PeakFileFormat::DeltaEncoding::None) {
        return false;
    }
    if (header.sampleFormat != PeakFileFormat::SampleFormat::Int16) {
        return false;
    }

    size_t offset = header.levelTableOffset;
    levels.clear();
    levels.reserve(header.levelCount);
    for (uint32 i = 0; i < header.levelCount; ++i) {
        PeakFileFormat::LevelInfo info;
        if (!readLevelInfo(rawData, static_cast<size_t>(fileSize), offset, info)) {
            return false;
        }
        levels.push_back({ info.offset, info.blockSize, info.blockCount });
    }

    sampleFormat = header.sampleFormat;
    sampleRate = header.sampleRate;
    channelCount = header.channelCount;
    totalSamples = header.totalSamples;
    baseBlockSize = header.baseBlockSize;
    return true;
}

bool PeakFile::isValid() const {
    return mappedFile != nullptr && !levels.empty();
}

uint32 PeakFile::getBestResolution(double samplesPerPixel) const {
    if (levels.empty()) {
        // Peak levels must be available before selecting.
        jassert(false);
        return 0;
    }

    const auto target = std::max(1.0, samplesPerPixel);
    auto bestIndex = static_cast<size_t>(0);
    auto bestDiff = std::abs(static_cast<double>(levels.front().blockSize) - target);
    for (size_t index = 1; index < levels.size(); ++index) {
        const auto diff = std::abs(static_cast<double>(levels[index].blockSize) - target);
        if (diff < bestDiff) {
            bestDiff = diff;
            bestIndex = index;
        }
    }
    return levels[bestIndex].blockSize;
}

bool PeakFile::readBlocksForRange(uint32 samplesPerBlock,
                                  uint64 startSample,
                                  uint64 endSample,
                                  std::vector<PeakBlock>& outBlocks) const {
    if (!isValid()) {
        // should be valid
        jassert(false);
        return false;
    }
    if (samplesPerBlock == 0 || endSample <= startSample) {
        // should be in range
        jassert(false);
        return false;
    }
    const LevelInfo* level = nullptr;
    for (const auto& info : levels) {
        if (info.blockSize == samplesPerBlock) {
            level = &info;
            break;
        }
    }
    if (level == nullptr) {
        // level should have been found
        jassert(false);
        return false;
    }

    const auto clampedEnd = std::min<uint64>(endSample, totalSamples);
    if (clampedEnd <= startSample) {
        // should be clamped
        jassert(false);
        return false;
    }

    const auto startBlock = static_cast<uint64>(startSample / level->blockSize);
    const auto endBlock = static_cast<uint64>((clampedEnd + level->blockSize - 1) / level->blockSize);
    const auto blockCount = static_cast<uint32>(std::max<uint64>(1, endBlock - startBlock));
    if (startBlock >= level->blockCount) {
        // should be clamped
        jassert(false);
        return false;
    }

    const auto clampedBlockCount = std::min<uint32>(blockCount,
                                                    static_cast<uint32>(level->blockCount - startBlock));
    const auto valuesPerBlock = static_cast<size_t>(channelCount) * 2;
    const auto bytesPerBlock = valuesPerBlock * sizeof(int16);
    const auto byteOffset = level->offset + startBlock * bytesPerBlock;
    const auto byteCount = static_cast<uint64>(clampedBlockCount) * bytesPerBlock;
    const auto fileSize = static_cast<uint64>(mappedFile->getSize());
    if (byteOffset + byteCount > fileSize) {
        // should be clamped
        jassert(false);
        return false;
    }

    const auto* rawData = static_cast<const uint8*>(mappedFile->getData());
    const auto* blockData = rawData + byteOffset;

    outBlocks.resize(static_cast<size_t>(clampedBlockCount) * channelCount);
    size_t dataOffset = 0;
    for (uint32 blockIndex = 0; blockIndex < clampedBlockCount; ++blockIndex) {
        for (uint32 channel = 0; channel < channelCount; ++channel) {
            auto& block = outBlocks[blockIndex * channelCount + channel];
            block.min = readInt16LE(blockData + dataOffset);
            dataOffset += sizeof(int16);
            block.max = readInt16LE(blockData + dataOffset);
            dataOffset += sizeof(int16);
        }
    }
    return true;
}
