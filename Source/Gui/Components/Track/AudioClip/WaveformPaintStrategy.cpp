#include "WaveformPaintStrategy.h"

#include <algorithm>
#include <cmath>

#include "Core/AudioClip/AudioClip.h"
#include "Utils/IO/AudioFile.h"
#include "Utils/Waveform/PeakFile.h"
#include "Gui/Style/Font.h"

namespace {
constexpr float kPerceptualK = 9.0f;
constexpr float kInt16ToFloat = 1.0f / 32768.0f;

float applyPerceptualMapping(float value) {
    const auto absValue = std::abs(value);
    const auto mapped = std::log1p(kPerceptualK * absValue) / std::log1p(kPerceptualK);
    return std::copysign(mapped, value);
}

float dequantizePeak(int16 value) {
    return static_cast<float>(value) * kInt16ToFloat;
}
} // namespace

void WaveformPaintStrategy::paint(juce::Graphics& g,
                                  const juce::Rectangle<float>& bounds,
                                  const AudioClip& clip,
                                  juce::Colour,
                                  float waveformScale,
                                  int64 viewStartSample,
                                  int64 viewEndSample) {
    const auto clipStart = clip.getSessionStartSample();
    const auto clipEnd = clip.getSessionEndSample();
    const auto visibleStart = std::max(clipStart, viewStartSample);
    const auto visibleEnd = std::min(clipEnd, viewEndSample);
    if (visibleEnd <= visibleStart) {
        return;
    }

    auto clipBounds = bounds.reduced(0.0f, 4.0f);
    const auto fill = juce::Colour(0xCCC8D9B8);
    g.setColour(fill);
    g.fillRoundedRectangle(clipBounds, 8.0f);

    g.setColour(juce::Colour(0xFF63A129));
    g.drawRoundedRectangle(clipBounds, 8.0f, 2.0f);

    const float midY = clipBounds.getCentreY();
    const float halfHeight = clipBounds.getHeight() * 0.45f * std::max(0.0f, waveformScale);
    const float width = clipBounds.getWidth();
    const float visibleX0 = ((static_cast<float>(visibleStart - clipStart)) / static_cast<float>(clipEnd - clipStart)) * width;
    const float visibleX1 = ((static_cast<float>(visibleEnd - clipStart)) / static_cast<float>(clipEnd - clipStart)) * width;
    auto visibleBounds = clipBounds.withTrimmedLeft(visibleX0).withWidth(std::max(1.0f, visibleX1 - visibleX0));

    g.drawLine(clipBounds.getX(), midY, clipBounds.getRight(), midY, 1.0f);

    const auto visibleSamples = visibleEnd - visibleStart;
    const float samplesPerPixel = static_cast<float>(visibleSamples) / std::max(1.0f, visibleBounds.getWidth());

    if (samplesPerPixel >= 128.0f) {
        if (auto audioFile = clip.getAudioFile()) {
            const auto peakFile = audioFile->getPeakFile();
            if (!peakFile || !peakFile->isValid()) {
                return;
            }
            const auto& level = peakFile->getBestLevel(samplesPerPixel);
            if (level.blockCount == 0 || level.blockSize == 0) {
                return;
            }

            const auto fileStart = clip.getFileStartSample() + (visibleStart - clipStart);
            const auto fileEnd = clip.getFileStartSample() + (visibleEnd - clipStart);
            if (fileEnd <= fileStart) {
                return;
            }

            const auto startBlock = static_cast<uint64>(fileStart / level.blockSize);
            const auto endBlock = static_cast<uint64>((fileEnd + level.blockSize - 1) / level.blockSize);
            const auto blockCount = static_cast<uint32>(std::max<uint64>(1, endBlock - startBlock));
            std::vector<PeakFile::PeakBlock> blocks;
            if (!peakFile->readBlocks(level, startBlock, blockCount, blocks)) {
                return;
            }

            const auto channelCount = peakFile->getChannelCount();
            const auto rangeSamples = static_cast<double>(fileEnd - fileStart);
            juce::Path path;
            std::vector<juce::Point<float>> minPoints;
            minPoints.reserve(blockCount);

            for (uint32 blockIndex = 0; blockIndex < blockCount; ++blockIndex) {
                const auto blockStart = (startBlock + blockIndex) * level.blockSize;
                const auto blockEnd = blockStart + level.blockSize;
                const auto blockCenter = static_cast<double>(blockStart + blockEnd) * 0.5;
                const auto normalizedX = static_cast<float>((blockCenter - fileStart) / rangeSamples);
                const auto clampedX = juce::jlimit(0.0f, 1.0f, normalizedX);
                const float x = visibleBounds.getX() + clampedX * visibleBounds.getWidth();

                float minValue = 1.0f;
                float maxValue = -1.0f;
                for (uint32 channel = 0; channel < channelCount; ++channel) {
                    const auto& block = blocks[blockIndex * channelCount + channel];
                    minValue = std::min(minValue, dequantizePeak(block.min));
                    maxValue = std::max(maxValue, dequantizePeak(block.max));
                }

                const auto mappedMax = applyPerceptualMapping(maxValue);
                const auto mappedMin = applyPerceptualMapping(minValue);
                const auto maxY = midY - (mappedMax * halfHeight);
                const auto minY = midY - (mappedMin * halfHeight);

                if (blockIndex == 0) {
                    path.startNewSubPath(x, maxY);
                } else {
                    path.lineTo(x, maxY);
                }
                minPoints.emplace_back(x, minY);
            }

            for (auto it = minPoints.rbegin(); it != minPoints.rend(); ++it) {
                path.lineTo(it->x, it->y);
            }

            const auto waveformColour = juce::Colour(0xFF63A129);
            g.setColour(waveformColour.withAlpha(0.7f));
            g.fillPath(path);
            g.setColour(waveformColour);
            g.strokePath(path, juce::PathStrokeType(1.0f));
        }
    } else if (samplesPerPixel > 1.0f) {
        const auto clipRelativeStart = visibleStart - clipStart;
        auto buffer = clip.read(clipRelativeStart, visibleSamples);
        if (buffer.getNumSamples() < 2) {
            return;
        }

        const int pixelCount = std::max(1, static_cast<int>(std::floor(visibleBounds.getWidth())));
        const float xStart = visibleBounds.getX();
        const float xStep = visibleBounds.getWidth() / static_cast<float>(pixelCount);
        const int totalSamples = buffer.getNumSamples();
        const float samplesPerPixelFloat = static_cast<float>(totalSamples) / static_cast<float>(pixelCount);
        juce::Path path;
        std::vector<juce::Point<float>> minPoints;
        minPoints.reserve(static_cast<size_t>(pixelCount));

        for (int pixelIndex = 0; pixelIndex < pixelCount; ++pixelIndex) {
            const int startSample = static_cast<int>(std::floor(pixelIndex * samplesPerPixelFloat));
            const int endSample = static_cast<int>(std::floor((pixelIndex + 1) * samplesPerPixelFloat));
            const int sampleCount = std::max(1, endSample - startSample);
            const int clampedStart = std::min(startSample, totalSamples - 1);
            const int clampedCount = std::min(sampleCount, totalSamples - clampedStart);

            float minValue = 1.0f;
            float maxValue = -1.0f;
            for (int channelIndex = 0; channelIndex < buffer.getNumChannels(); ++channelIndex) {
                const float* samples = buffer.getReadPointer(channelIndex);
                auto range = juce::FloatVectorOperations::findMinAndMax(samples + clampedStart, clampedCount);
                minValue = std::min(minValue, range.getStart());
                maxValue = std::max(maxValue, range.getEnd());
            }

            const float mappedMax = applyPerceptualMapping(maxValue);
            const float mappedMin = applyPerceptualMapping(minValue);
            const float x = xStart + xStep * static_cast<float>(pixelIndex);
            const float maxY = midY - (mappedMax * halfHeight);
            const float minY = midY - (mappedMin * halfHeight);

            if (pixelIndex == 0) {
                path.startNewSubPath(x, maxY);
            } else {
                path.lineTo(x, maxY);
            }
            minPoints.emplace_back(x, minY);
        }

        for (auto it = minPoints.rbegin(); it != minPoints.rend(); ++it) {
            path.lineTo(it->x, it->y);
        }

        const auto waveformColour = juce::Colour(0xFF63A129);
        g.setColour(waveformColour.withAlpha(0.7f));
        g.fillPath(path);
        g.setColour(waveformColour);
        g.strokePath(path, juce::PathStrokeType(1.0f));
    } else {
        const auto clipRelativeStart = visibleStart - clipStart;
        auto buffer = clip.read(clipRelativeStart, visibleSamples);
        if (buffer.getNumSamples() < 2) {
            return;
        }

        juce::Path path;
        path.preallocateSpace(static_cast<int>(buffer.getNumSamples()));
        const float xStart = visibleBounds.getX();
        const float xStep = visibleBounds.getWidth() / static_cast<float>(buffer.getNumSamples() - 1);
        const float* samples = buffer.getReadPointer(0);
        path.startNewSubPath(xStart, midY - (applyPerceptualMapping(samples[0]) * halfHeight));
        for (int i = 1; i < buffer.getNumSamples(); ++i) {
            const float x = xStart + xStep * static_cast<float>(i);
            path.lineTo(x, midY - (applyPerceptualMapping(samples[i]) * halfHeight));
        }

        g.setColour(juce::Colour(0xFF63A129));
        g.strokePath(path, juce::PathStrokeType(1.0f));
    }

    g.setFont(Fonts::p(10.0f, Fonts::Weight::Regular));
    g.setColour(juce::Colours::darkgrey);
    auto titleBounds = juce::Rectangle<int>(clipBounds.getSmallestIntegerContainer())
                           .reduced(6, 6)
                           .removeFromLeft(100)
                           .removeFromTop(15);
    g.drawText("Clip Name", titleBounds, juce::Justification::topLeft, true);
}
