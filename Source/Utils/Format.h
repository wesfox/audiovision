#pragma once
#include <string_view>
#include <cstdint>

enum class ChannelsFormat : std::uint8_t
{
    Mono,      // 1
    Stereo,    // 2
    LCR,       // 3
    Quad,      // 4
    FiveZero,  // 5 (5.0)
    FiveOne,   // 6 (5.1)
    SevenZero, // 7 (7.0)
    SevenOne   // 8 (7.1)
};

constexpr int ChannelCount(const ChannelsFormat f) noexcept
{
    switch (f)
    {
        case ChannelsFormat::Mono:       return 1;
        case ChannelsFormat::Stereo:     return 2;
        case ChannelsFormat::LCR:        return 3;
        case ChannelsFormat::Quad:       return 4;
        case ChannelsFormat::FiveZero:   return 5;
        case ChannelsFormat::FiveOne:    return 6;
        case ChannelsFormat::SevenZero:  return 7;
        case ChannelsFormat::SevenOne:   return 8;
    }
    return 0;
}

inline String ChannelsFormatName(const ChannelsFormat f) noexcept
{
    switch (f)
    {
        case ChannelsFormat::Mono:       return "Mono";
        case ChannelsFormat::Stereo:     return "Stereo";
        case ChannelsFormat::LCR:        return "LCR";
        case ChannelsFormat::Quad:       return "Quad";
        case ChannelsFormat::FiveZero:   return "5.0";
        case ChannelsFormat::FiveOne:    return "5.1";
        case ChannelsFormat::SevenZero:  return "7.0";
        case ChannelsFormat::SevenOne:   return "7.1";
    }
    return "Unknown";
}

inline const juce::AudioChannelSet& GetAudioChannelSet (const ChannelsFormat f) noexcept
{
    static const juce::AudioChannelSet mono       = juce::AudioChannelSet::mono();
    static const juce::AudioChannelSet stereo     = juce::AudioChannelSet::stereo();
    static const juce::AudioChannelSet lcr        = juce::AudioChannelSet::createLCR();
    static const juce::AudioChannelSet quad       = juce::AudioChannelSet::quadraphonic();
    static const juce::AudioChannelSet fiveZero   = juce::AudioChannelSet::create5point0();
    static const juce::AudioChannelSet fiveOne    = juce::AudioChannelSet::create5point1();
    static const juce::AudioChannelSet sevenZero  = juce::AudioChannelSet::create7point0();
    static const juce::AudioChannelSet sevenOne   = juce::AudioChannelSet::create7point1();

    switch (f)
    {
        case ChannelsFormat::Mono:       return mono;
        case ChannelsFormat::Stereo:     return stereo;
        case ChannelsFormat::LCR:        return lcr;
        case ChannelsFormat::Quad:       return quad;
        case ChannelsFormat::FiveZero:   return fiveZero;
        case ChannelsFormat::FiveOne:    return fiveOne;
        case ChannelsFormat::SevenZero:  return sevenZero;
        case ChannelsFormat::SevenOne:   return sevenOne;
    }

    jassertfalse;
    return mono;
}