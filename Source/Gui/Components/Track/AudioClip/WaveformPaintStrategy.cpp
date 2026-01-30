#include "WaveformPaintStrategy.h"

#include <algorithm>

#include "Core/AudioClip/AudioClip.h"
#include "Utils/IO/AudioFile.h"
#include "Gui/Style/Font.h"

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

    const auto sampleRate = clip.getSampleRate();
    const auto visibleSamples = visibleEnd - visibleStart;
    const float samplesPerPixel = static_cast<float>(visibleSamples) / std::max(1.0f, visibleBounds.getWidth());

    if (samplesPerPixel > 4.0f) {
        if (auto audioFile = clip.getAudioFile()) {
            if (auto* thumbnail = audioFile->getThumbnail()) {
                const auto fileStart = clip.getFileStartSample() + (visibleStart - clipStart);
                const auto fileEnd = clip.getFileStartSample() + (visibleEnd - clipStart);
                const double startTime = static_cast<double>(fileStart) / sampleRate;
                const double endTime = static_cast<double>(fileEnd) / sampleRate;
                const auto drawArea = visibleBounds.getSmallestIntegerContainer();
                g.setColour(juce::Colour(0xFF63A129));
                thumbnail->drawChannel(g, drawArea, startTime, endTime, 0, waveformScale);
            }
        }
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
        path.startNewSubPath(xStart, midY - (samples[0] * halfHeight));
        for (int i = 1; i < buffer.getNumSamples(); ++i) {
            const float x = xStart + xStep * static_cast<float>(i);
            path.lineTo(x, midY - (samples[i] * halfHeight));
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
