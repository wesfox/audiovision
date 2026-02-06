#include "WaveformPaintStrategy.h"

#include <algorithm>
#include <cmath>

#include "Core/AudioClip/AudioClip.h"
#include "Utils/IO/AudioFile.h"
#include "Utils/Waveform/PeakFileBuilder.h"
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

struct PixelPeak {
    float min = 0.0f;
    float max = 0.0f;
    bool hasValue = false;
};


// Aggregate peak blocks into per-pixel min/max buckets.
void aggregateBlocksToPixels(const std::vector<PeakFile::PeakBlock>& blocks,
                             uint32 channelCount,
                             uint64 startBlock,
                             uint32 samplesPerBlock,
                             int64 fileStart,
                             int64 fileEnd,
                             int pixelCount,
                             std::vector<PixelPeak>& outPeaks) {
    outPeaks.clear();
    if (pixelCount <= 0 || channelCount == 0 || blocks.empty()) {
        return;
    }

    const auto blockCount = static_cast<uint32>(blocks.size() / channelCount);
    if (blockCount == 0) {
        return;
    }

    const auto rangeSamples = fileEnd - fileStart;
    if (rangeSamples <= 0) {
        return;
    }

    outPeaks.resize(static_cast<size_t>(pixelCount));
    const auto lastBlock = startBlock + blockCount;

    for (int pixelIndex = 0; pixelIndex < pixelCount; ++pixelIndex) {
        const auto startRatio = static_cast<double>(pixelIndex) / static_cast<double>(pixelCount);
        const auto endRatio = static_cast<double>(pixelIndex + 1) / static_cast<double>(pixelCount);
        auto pixelStartSample = fileStart + static_cast<int64>(std::floor(startRatio * rangeSamples));
        auto pixelEndSample = fileStart + static_cast<int64>(std::floor(endRatio * rangeSamples));
        if (pixelEndSample <= pixelStartSample) {
            pixelEndSample = pixelStartSample + 1;
        }

        const auto pixelStartBlock = static_cast<uint64>(pixelStartSample / samplesPerBlock);
        const auto pixelEndBlock = static_cast<uint64>((pixelEndSample + samplesPerBlock - 1) / samplesPerBlock);
        const auto clampedStartBlock = std::max<uint64>(pixelStartBlock, startBlock);
        const auto clampedEndBlock = std::min<uint64>(pixelEndBlock, lastBlock);
        if (clampedEndBlock <= clampedStartBlock) {
            continue;
        }

        float minValue = 1.0f;
        float maxValue = -1.0f;
        for (uint64 blockIndex = clampedStartBlock; blockIndex < clampedEndBlock; ++blockIndex) {
            const auto localIndex = static_cast<size_t>(blockIndex - startBlock);
            for (uint32 channel = 0; channel < channelCount; ++channel) {
                const auto& block = blocks[localIndex * channelCount + channel];
                minValue = std::min(minValue, dequantizePeak(block.min));
                maxValue = std::max(maxValue, dequantizePeak(block.max));
            }
        }

        outPeaks[static_cast<size_t>(pixelIndex)] = { minValue, maxValue, true };
    }
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

    // use cache
    if (std::round(samplesPerPixel) >= PeakFileBuilder::getMinSamplesPerBlock()) {
        if (auto audioFile = clip.getAudioFile()) {
            const auto peakFile = audioFile->getPeakFile();
            if (!peakFile || !peakFile->isValid()) {
                return;
            }
            const auto samplesPerBlock = peakFile->getBestResolution(samplesPerPixel);
            if (samplesPerBlock == 0) {
                return;
            }

            const auto fileStart = clip.getFileStartSample() + (visibleStart - clipStart);
            auto fileEnd = clip.getFileStartSample() + (visibleEnd - clipStart);
            const auto totalSamples = static_cast<int64>(peakFile->getTotalSamples());
            if (totalSamples > 0 && fileEnd > totalSamples) {
                fileEnd = totalSamples;
            }
            if (fileEnd <= fileStart) {
                return;
            }

            std::vector<PeakFile::PeakBlock> blocks;
            if (!peakFile->readBlocksForRange(static_cast<uint32>(samplesPerBlock),
                                              static_cast<uint64>(fileStart),
                                              static_cast<uint64>(fileEnd),
                                              blocks)) {
                return;
            }

            const auto channelCount = peakFile->getChannelCount();
            juce::Path path;
            std::vector<juce::Point<float>> minPoints;
            const int pixelCount = std::max(1, static_cast<int>(std::floor(visibleBounds.getWidth())));
            minPoints.reserve(static_cast<size_t>(pixelCount));

            std::vector<PixelPeak> pixelPeaks;
            const auto startBlock = static_cast<uint64>(fileStart / samplesPerBlock);
            aggregateBlocksToPixels(blocks,
                                    channelCount,
                                    startBlock,
                                    static_cast<uint32>(samplesPerBlock),
                                    fileStart,
                                    fileEnd,
                                    pixelCount,
                                    pixelPeaks);

            const float xStart = visibleBounds.getX();
            const float xStep = visibleBounds.getWidth() / static_cast<float>(pixelCount);
            float previousMin = 0.0f;
            float previousMax = 0.0f;

            for (int pixelIndex = 0; pixelIndex < pixelCount; ++pixelIndex) {
                const auto& peak = pixelPeaks[static_cast<size_t>(pixelIndex)];
                const float minValue = peak.hasValue ? peak.min : previousMin;
                const float maxValue = peak.hasValue ? peak.max : previousMax;
                previousMin = minValue;
                previousMax = maxValue;

                const float x = xStart + xStep * static_cast<float>(pixelIndex);

                const auto mappedMax = applyPerceptualMapping(maxValue);
                const auto mappedMin = applyPerceptualMapping(minValue);
                const auto maxY = midY - (mappedMax * halfHeight);
                const auto minY = midY - (mappedMin * halfHeight);

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
        }
    // use raw sample but as there is still multiple samples per pixel, we need to mean them (still min/max)
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
    // space between sample is >= than one pixel so we need to draw each sample (no more min/max)
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
