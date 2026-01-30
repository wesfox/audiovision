#pragma once

#include "AudioClipPaintStrategy.h"

/// Paints a clip waveform from cached samples.
class WaveformPaintStrategy : public AudioClipPaintStrategy {
public:
    void paint(juce::Graphics& g,
               const juce::Rectangle<float>& bounds,
               const AudioClip& clip,
               juce::Colour colour,
               float waveformScale,
               int64 viewStartSample,
               int64 viewEndSample) override;
};
