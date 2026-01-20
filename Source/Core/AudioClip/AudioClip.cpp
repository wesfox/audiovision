#include "AudioClip.h"
#include <Utils/Uuid.h>

AudioClip::AudioClip(const std::shared_ptr<AudioFile> &audioFile)
    : audioFile(std::move(audioFile)),
      id(uuid::generate_uuid_v4()),
      name(""),
      color(juce::Colours::white),
      fileStartSample(0),
      fadeIn(),
      fadeOut(),
      sessionStartSample(0),
      sessionEndSample(48000),
      clipLength(48000),
      sampleRate(48000),
      format(ChannelsFormat::Mono),
      muted(false)
{}

String AudioClip::getId() const
{
    return id;
}

String AudioClip::getName() const
{
    return name;
}

juce::Colour AudioClip::getColor() const
{
    return color;
}

int64 AudioClip::getFileStartSample() const
{
    return fileStartSample;
}

int64 AudioClip::getSessionStartSample() const
{
    return sessionStartSample;
}

int64 AudioClip::getSessionEndSample() const
{
    return sessionEndSample;
}

int64 AudioClip::getClipLength() const
{
    return sessionEndSample - sessionStartSample;
}

double AudioClip::getSampleRate() const
{
    return sampleRate;
}

ChannelsFormat AudioClip::getFormat() const
{
    return format;
}

std::vector<float> AudioClip::getWaveformData()
{
    // Empty implementation
    return std::vector<float>();
}

juce::AudioBuffer<float> AudioClip::read(int64 startSample, int64 numberOfSamples)
{
    return audioFile->read(fileStartSample + startSample, numberOfSamples);
}

void AudioClip::move(int64 newSessionStartSample)
{
    const auto currentClipLength = getClipLength();
    sessionStartSample = newSessionStartSample;
    sessionEndSample = newSessionStartSample + currentClipLength;
}

std::unique_ptr<AudioClip> AudioClip::split(int64 splitSample)
{
    const int64 originalSessionStart = sessionStartSample;
    const int64 originalSessionEnd = sessionEndSample;
    const int64 clampedSplit = splitSample < originalSessionStart
        ? originalSessionStart
        : (splitSample > originalSessionEnd ? originalSessionEnd : splitSample);
    const int64 splitOffset = clampedSplit - originalSessionStart;

    auto newClip = std::make_unique<AudioClip>(audioFile);
    AudioClip& newClipRef = *newClip;
    newClipRef.name = name;
    newClipRef.color = color;
    newClipRef.format = format;
    newClipRef.muted = muted;
    // only fade out is passed through
    newClipRef.fadeOut = fadeOut;
    newClipRef.fileStartSample = fileStartSample + splitOffset;
    newClipRef.sampleRate = sampleRate;
    newClipRef.setSessionStartSample(clampedSplit);
    newClipRef.setSessionEndSample(originalSessionEnd);

    // original fade out must be reset
    fadeOut.remove();

    sessionEndSample = clampedSplit;

    return newClip;
}

void AudioClip::trimHead(int64 newSessionStartSample, bool keepFade)
{
    // Empty implementation
}

void AudioClip::trimTail(int64 newSessionEndSample, bool keepFade)
{
    // Empty implementation
}

void AudioClip::setSessionStartSample(int64 newSessionStartSample)
{
    sessionStartSample = newSessionStartSample;
    clipLength = sessionEndSample - sessionStartSample;
}

void AudioClip::setSessionEndSample(int64 newSessionEndSample)
{
    sessionEndSample = newSessionEndSample;
    clipLength = sessionEndSample - sessionStartSample;
}

void AudioClip::setSampleRate(double newSampleRate)
{
    sampleRate = newSampleRate;
}

void AudioClip::setFileStartSample(int64 newFileStartSample)
{
    fileStartSample = newFileStartSample;
}
