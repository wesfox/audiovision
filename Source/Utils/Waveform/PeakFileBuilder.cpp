#include "PeakFileBuilder.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace {
constexpr float kFloatToInt16 = 32767.0f;

int16 quantizePeak(float value) {
    const auto clamped = juce::jlimit(-1.0f, 1.0f, value);
    const auto scaled = std::lround(clamped * kFloatToInt16);
    return static_cast<int16>(juce::jlimit(-32767L, 32767L, scaled));
}

struct LevelBuildInfo {
    PeakFileFormat::LevelInfo info;
    std::vector<int16> data;
};

void writeInt16VectorLE(juce::OutputStream& output, const std::vector<int16>& data) {
    if (data.empty()) {
        return;
    }
    if (!juce::ByteOrder::isBigEndian()) {
        output.write(data.data(), static_cast<int>(data.size() * sizeof(int16)));
        return;
    }

    std::vector<int16> swapped;
    swapped.reserve(data.size());
    for (auto value : data) {
        const auto swappedValue = juce::ByteOrder::swapIfBigEndian(static_cast<uint16>(value));
        swapped.push_back(static_cast<int16>(swappedValue));
    }
    output.write(swapped.data(), static_cast<int>(swapped.size() * sizeof(int16)));
}

std::vector<PeakFileFormat::LevelInfo> buildLevelInfos(uint64 totalSamples,
                                                       uint32 baseBlockSize,
                                                       uint32 targetMaxBlocks) {
    std::vector<PeakFileFormat::LevelInfo> infos;
    uint32 blockSize = baseBlockSize;
    while (blockSize > 0) {
        const auto blockCount = static_cast<uint32>((totalSamples + blockSize - 1) / blockSize);
        infos.push_back({ 0, blockSize, blockCount });
        if (blockCount <= targetMaxBlocks) {
            break;
        }
        if (blockSize > std::numeric_limits<uint32>::max() / 2) {
            break;
        }
        blockSize *= 2;
    }
    return infos;
}
} // namespace

bool PeakFileBuilder::build(juce::AudioFormatReader& reader,
                            const juce::File& peakFilePath,
                            const BuildOptions& options) {
    if (options.compressionHook) {
        // Compression is not implemented yet.
        jassert(false);
        return false;
    }
    const auto totalSamples = static_cast<uint64>(reader.lengthInSamples);
    const auto channelCount = static_cast<uint32>(reader.numChannels);
    if (channelCount == 0 || totalSamples == 0) {
        return false;
    }

    const auto levelInfos = buildLevelInfos(totalSamples,
                                            options.baseBlockSize,
                                            options.targetMaxBlocks);
    const auto levelCount = static_cast<uint32>(levelInfos.size());
    if (levelCount == 0) {
        return false;
    }

    PeakFileFormat::Header header;
    header.sampleRate = static_cast<uint32>(std::lround(reader.sampleRate));
    header.channelCount = channelCount;
    header.totalSamples = totalSamples;
    header.baseBlockSize = options.baseBlockSize;
    header.levelCount = levelCount;
    header.sampleFormat = options.sampleFormat;
    header.compression = PeakFileFormat::Compression::None;
    header.deltaEncoding = PeakFileFormat::DeltaEncoding::None;
    header.levelTableOffset = PeakFileFormat::kHeaderSize;

    auto outputStream = peakFilePath.createOutputStream();
    if (outputStream == nullptr) {
        return false;
    }

    const auto levelTableSize = static_cast<uint64>(levelCount) * PeakFileFormat::kLevelInfoSize;
    uint64 dataOffset = PeakFileFormat::kHeaderSize + levelTableSize;

    std::vector<PeakFileFormat::LevelInfo> resolvedInfos = levelInfos;
    for (auto& info : resolvedInfos) {
        info.offset = dataOffset;
        const auto bytesPerBlock = static_cast<uint64>(channelCount) * sizeof(int16) * 2;
        const auto levelBytes = bytesPerBlock * info.blockCount;
        dataOffset += levelBytes;
    }

    PeakFileFormat::writeUint32LE(*outputStream, header.magic);
    PeakFileFormat::writeUint16LE(*outputStream, header.version);
    PeakFileFormat::writeUint16LE(*outputStream, header.headerSize);
    PeakFileFormat::writeUint32LE(*outputStream, header.sampleRate);
    PeakFileFormat::writeUint32LE(*outputStream, header.channelCount);
    PeakFileFormat::writeUint64LE(*outputStream, header.totalSamples);
    PeakFileFormat::writeUint32LE(*outputStream, header.baseBlockSize);
    PeakFileFormat::writeUint32LE(*outputStream, header.levelCount);
    outputStream->writeByte(static_cast<char>(header.sampleFormat));
    outputStream->writeByte(static_cast<char>(header.compression));
    outputStream->writeByte(static_cast<char>(header.deltaEncoding));
    outputStream->write(header.reserved, sizeof(header.reserved));
    PeakFileFormat::writeUint64LE(*outputStream, header.levelTableOffset);

    for (const auto& info : resolvedInfos) {
        PeakFileFormat::writeUint64LE(*outputStream, info.offset);
        PeakFileFormat::writeUint32LE(*outputStream, info.blockSize);
        PeakFileFormat::writeUint32LE(*outputStream, info.blockCount);
    }

    const auto valuesPerBlock = static_cast<int>(channelCount) * 2;
    LevelBuildInfo currentLevel;
    currentLevel.info = resolvedInfos.front();
    currentLevel.data.resize(static_cast<size_t>(currentLevel.info.blockCount) * valuesPerBlock);

    const int64 blockSize = currentLevel.info.blockSize;
    const int64 maxChunkSamples = blockSize * 256;
    juce::AudioBuffer<float> buffer(static_cast<int>(channelCount),
                                    static_cast<int>(maxChunkSamples));

    std::vector<float> blockMin(channelCount, std::numeric_limits<float>::max());
    std::vector<float> blockMax(channelCount, std::numeric_limits<float>::lowest());
    uint64 currentBlockIndex = 0;
    int64 blockSamplesFilled = 0;
    int64 samplePosition = 0;
    while (samplePosition < static_cast<int64>(totalSamples)) {
        const auto remaining = static_cast<int64>(totalSamples) - samplePosition;
        const auto samplesToRead = static_cast<int>(std::min<int64>(remaining, maxChunkSamples));
        reader.read(&buffer, 0, samplesToRead, samplePosition, true, true);

        int64 localOffset = 0;
        while (localOffset < samplesToRead) {
            const int64 globalSample = samplePosition + localOffset;
            const int64 blockOffset = globalSample % blockSize;
            const int64 samplesInBlock = std::min<int64>(blockSize - blockOffset,
                                                         samplesToRead - localOffset);
            for (uint32 channel = 0; channel < channelCount; ++channel) {
                const float* channelData = buffer.getReadPointer(static_cast<int>(channel));
                auto range = juce::FloatVectorOperations::findMinAndMax(channelData + localOffset,
                                                                        static_cast<int>(samplesInBlock));
                blockMin[channel] = std::min(blockMin[channel], range.getStart());
                blockMax[channel] = std::max(blockMax[channel], range.getEnd());
            }
            blockSamplesFilled += samplesInBlock;
            localOffset += samplesInBlock;

            const bool blockComplete = blockSamplesFilled >= blockSize
                                       || (globalSample + samplesInBlock) >= static_cast<int64>(totalSamples);
            if (!blockComplete) {
                continue;
            }

            const auto blockIndex = static_cast<size_t>(currentBlockIndex);
            auto writeOffset = blockIndex * valuesPerBlock;
            for (uint32 channel = 0; channel < channelCount; ++channel) {
                currentLevel.data[writeOffset++] = quantizePeak(blockMin[channel]);
                currentLevel.data[writeOffset++] = quantizePeak(blockMax[channel]);
                blockMin[channel] = std::numeric_limits<float>::max();
                blockMax[channel] = std::numeric_limits<float>::lowest();
            }
            ++currentBlockIndex;
            blockSamplesFilled = 0;
        }

        samplePosition += samplesToRead;
    }

    outputStream->setPosition(static_cast<int64>(currentLevel.info.offset));
    writeInt16VectorLE(*outputStream, currentLevel.data);

    LevelBuildInfo previousLevel = std::move(currentLevel);
    for (size_t levelIndex = 1; levelIndex < resolvedInfos.size(); ++levelIndex) {
        LevelBuildInfo nextLevel;
        nextLevel.info = resolvedInfos[levelIndex];
        nextLevel.data.resize(static_cast<size_t>(nextLevel.info.blockCount) * valuesPerBlock);

        for (uint32 blockIndex = 0; blockIndex < nextLevel.info.blockCount; ++blockIndex) {
            const auto firstChild = static_cast<size_t>(blockIndex) * 2;
            const auto secondChild = firstChild + 1;

            auto writeOffset = static_cast<size_t>(blockIndex) * valuesPerBlock;
            for (uint32 channel = 0; channel < channelCount; ++channel) {
                auto minValue = previousLevel.data[firstChild * valuesPerBlock + channel * 2];
                auto maxValue = previousLevel.data[firstChild * valuesPerBlock + channel * 2 + 1];
                if (secondChild < previousLevel.info.blockCount) {
                    const auto secondMin = previousLevel.data[secondChild * valuesPerBlock + channel * 2];
                    const auto secondMax = previousLevel.data[secondChild * valuesPerBlock + channel * 2 + 1];
                    minValue = std::min(minValue, secondMin);
                    maxValue = std::max(maxValue, secondMax);
                }
                nextLevel.data[writeOffset++] = minValue;
                nextLevel.data[writeOffset++] = maxValue;
            }
        }

        outputStream->setPosition(static_cast<int64>(nextLevel.info.offset));
        writeInt16VectorLE(*outputStream, nextLevel.data);

        previousLevel = std::move(nextLevel);
    }

    outputStream->flush();
    return outputStream->getStatus().wasOk();
}

bool PeakFileBuilder::build(juce::AudioFormatReader& reader,
                            const juce::File& peakFilePath) {
    return build(reader, peakFilePath, BuildOptions{});
}
