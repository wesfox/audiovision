#pragma once

#include <JuceHeader.h>

class AudioClip;

/// Defines how a clip is rendered inside its bounds.
class AudioClipPaintStrategy {
public:
    virtual ~AudioClipPaintStrategy() = default;

    /// Render the clip inside the provided bounds.
    /// @param g target graphics context
    /// @param bounds area to draw into
    /// @param clip clip model to visualize
    /// @param colour base colour for the clip
    /// @param waveformScale vertical scale applied to the waveform
    /// @param viewStartSample view start sample in session
    /// @param viewEndSample view end sample in session
    virtual void paint(juce::Graphics& g,
                       const juce::Rectangle<float>& bounds,
                       const AudioClip& clip,
                       juce::Colour colour,
                       float waveformScale,
                       int64 viewStartSample,
                       int64 viewEndSample) = 0;
};
