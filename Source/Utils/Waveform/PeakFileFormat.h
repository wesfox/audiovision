#pragma once

#include <JuceHeader.h>

#include <cstring>

namespace PeakFileFormat {
constexpr uint32 kMagic = 0x5045414B; // 'PEAK'
constexpr uint16 kVersion = 1;
constexpr uint16 kHeaderSize = 48;
constexpr uint64 kLevelInfoSize = 16;

enum class SampleFormat : uint8 {
    Int16 = 0
};

enum class Compression : uint8 {
    None = 0
};

enum class DeltaEncoding : uint8 {
    None = 0
};

struct Header {
    uint32 magic = kMagic;
    uint16 version = kVersion;
    uint16 headerSize = kHeaderSize;
    uint32 sampleRate = 0;
    uint32 channelCount = 0;
    uint64 totalSamples = 0;
    uint32 baseBlockSize = 0;
    uint32 levelCount = 0;
    SampleFormat sampleFormat = SampleFormat::Int16;
    Compression compression = Compression::None;
    DeltaEncoding deltaEncoding = DeltaEncoding::None;
    uint8 reserved[5] = { 0 };
    uint64 levelTableOffset = kHeaderSize;
};

struct LevelInfo {
    uint64 offset = 0;
    uint32 blockSize = 0;
    uint32 blockCount = 0;
};

inline void writeUint16LE(juce::OutputStream& out, uint16 value) {
    out.writeShort(static_cast<short>(juce::ByteOrder::swapIfBigEndian(value)));
}

inline void writeUint32LE(juce::OutputStream& out, uint32 value) {
    out.writeInt(static_cast<int>(juce::ByteOrder::swapIfBigEndian(value)));
}

inline void writeUint64LE(juce::OutputStream& out, uint64 value) {
    auto swapped = juce::ByteOrder::swapIfBigEndian(value);
    out.write(&swapped, sizeof(uint64));
}

inline bool readUint16LE(const uint8* data, size_t size, size_t& offset, uint16& value) {
    if (offset + sizeof(uint16) > size) {
        return false;
    }
    uint16 raw = 0;
    std::memcpy(&raw, data + offset, sizeof(uint16));
    value = juce::ByteOrder::swapIfBigEndian(raw);
    offset += sizeof(uint16);
    return true;
}

inline bool readUint32LE(const uint8* data, size_t size, size_t& offset, uint32& value) {
    if (offset + sizeof(uint32) > size) {
        return false;
    }
    uint32 raw = 0;
    std::memcpy(&raw, data + offset, sizeof(uint32));
    value = juce::ByteOrder::swapIfBigEndian(raw);
    offset += sizeof(uint32);
    return true;
}

inline bool readUint64LE(const uint8* data, size_t size, size_t& offset, uint64& value) {
    if (offset + sizeof(uint64) > size) {
        return false;
    }
    uint64 raw = 0;
    std::memcpy(&raw, data + offset, sizeof(uint64));
    value = juce::ByteOrder::swapIfBigEndian(raw);
    offset += sizeof(uint64);
    return true;
}
} // namespace PeakFileFormat
