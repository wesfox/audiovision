#pragma once

#include <JuceHeader.h>

#include "Components/Track/AudioClip/AudioClipPaintStrategy.h"

class AudioClip;

class AudioClipComponent : public juce::Component,
                           private juce::ChangeListener {
public:
    explicit AudioClipComponent(const AudioClip& clip, juce::Colour colour);
    ~AudioClipComponent() override;

    const AudioClip& getClip() const;

    void paint(juce::Graphics& g) override;

    /// Replace the current paint strategy for the clip.
    /// @param newStrategy painter responsible for rendering the clip
    void setPaintStrategy(std::unique_ptr<AudioClipPaintStrategy> newStrategy);

    /// Update the waveform vertical scale.
    /// @param scale waveform scale multiplier
    void setWaveformScale(float scale);

    /// Update the view range in session samples, for partial rendering.
    /// @param startSample view start sample in session
    /// @param endSample view end sample in session
    void setViewRange(int64 startSample, int64 endSample);

private:
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    const AudioClip& clip;
    const juce::Colour colour;
    std::unique_ptr<AudioClipPaintStrategy> paintStrategy;
    float waveformScale = 1.0f;
    int64 viewStartSample = 0;
    int64 viewEndSample = 0;
    juce::AudioThumbnail* thumbnail = nullptr;
};
