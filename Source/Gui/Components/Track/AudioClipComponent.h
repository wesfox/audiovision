#pragma once

#include <JuceHeader.h>

#include "Components/Track/AudioClip/AudioClipPaintStrategy.h"

class AudioClip;

class AudioClipComponent : public juce::Component {
public:
    explicit AudioClipComponent(const AudioClip& clip, juce::Colour colour);
    ~AudioClipComponent() override;

    const AudioClip& getClip() const;

    void paint(juce::Graphics& g) override;
    void mouseDoubleClick(const juce::MouseEvent& event) override;
    void mouseMove(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

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

    /// Set whether the clip is selected.
    /// @param shouldSelect true when selected
    void setSelected(bool shouldSelect);

    /// True when the clip is selected.
    bool isSelected() const;

    /// Notify when the clip is double-clicked.
    /// @param component clip component clicked
    /// @param sample sample position under the click
    /// @param extendSelection true when shift is held
    std::function<void(AudioClipComponent& component, int64 sample, bool extendSelection)> onDoubleClick;

    /// Notify when a trim drag occurs.
    /// @param clipId clip identifier
    /// @param sample new session sample for the trim
    /// @param trimHead true when trimming the head
    /// @param commit true when drag ends
    std::function<void(const String& clipId, int64 sample, bool trimHead, bool commit)> onTrimDrag;

    /// Forward mouse down events when the clip does not consume them.
    std::function<void(const juce::MouseEvent& event)> onMousePassthroughDown;

    /// Forward mouse drag events when the clip does not consume them.
    std::function<void(const juce::MouseEvent& event)> onMousePassthroughDrag;

    /// Forward mouse up events when the clip does not consume them.
    std::function<void(const juce::MouseEvent& event)> onMousePassthroughUp;

    /// Check whether the mouse event targets a trim handle.
    /// @param event mouse event to test
    bool isTrimHandleHit(const juce::MouseEvent& event) const;

    /// True when a trim drag is active.
    bool isTrimmingActive() const;

private:
    const AudioClip& clip;
    const juce::Colour colour;
    std::unique_ptr<AudioClipPaintStrategy> paintStrategy;
    float waveformScale = 1.0f;
    int64 viewStartSample = 0;
    int64 viewEndSample = 0;
    bool isSelectedClip = false;
    enum class TrimHandle {
        None,
        Head,
        Tail
    };
    TrimHandle activeTrimHandle = TrimHandle::None;
    bool isTrimming = false;
};
