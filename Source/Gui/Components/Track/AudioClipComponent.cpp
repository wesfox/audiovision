#include "AudioClipComponent.h"

#include "Components/Track/AudioClip/WaveformPaintStrategy.h"
#include "Core/AudioClip/AudioClip.h"

AudioClipComponent::AudioClipComponent(const AudioClip& clip, const juce::Colour colour)
    : clip(clip),
      colour(colour),
      paintStrategy(std::make_unique<WaveformPaintStrategy>()) {
}

AudioClipComponent::~AudioClipComponent() {
}

const AudioClip& AudioClipComponent::getClip() const {
    return clip;
}

void AudioClipComponent::paint(juce::Graphics& g) {
    if (!paintStrategy) {
        return;
    }
    paintStrategy->paint(g,
                         getLocalBounds().toFloat(),
                         clip,
                         colour,
                         waveformScale,
                         viewStartSample,
                         viewEndSample);

    if (isSelectedClip) {
        auto clipBounds = getLocalBounds().toFloat().reduced(0.0f, 4.0f);
        g.setColour(juce::Colour::fromString("#99000000"));
        g.fillRoundedRectangle(clipBounds, 8.0f);
        g.setColour(juce::Colour(0xFFB400FF));
        g.drawRoundedRectangle(clipBounds, 8.0f, 2.0f);
    }
}

void AudioClipComponent::setPaintStrategy(std::unique_ptr<AudioClipPaintStrategy> newStrategy) {
    paintStrategy = std::move(newStrategy);
    repaint();
}

void AudioClipComponent::setWaveformScale(float scale) {
    waveformScale = scale;
    repaint();
}

void AudioClipComponent::setViewRange(int64 startSample, int64 endSample) {
    viewStartSample = startSample;
    viewEndSample = endSample;
    repaint();
}

void AudioClipComponent::setSelected(bool shouldSelect) {
    if (isSelectedClip == shouldSelect) {
        return;
    }
    isSelectedClip = shouldSelect;
    if (isSelectedClip) {
        toFront(false);
    }
    repaint();
}

bool AudioClipComponent::isSelected() const {
    return isSelectedClip;
}

bool AudioClipComponent::isTrimHandleHit(const juce::MouseEvent& event) const {
    const auto localX = event.getEventRelativeTo(this).position.x;
    const auto bounds = getLocalBounds().toFloat();
    const float handleWidth = 6.0f;
    return localX <= handleWidth || localX >= bounds.getWidth() - handleWidth;
}

bool AudioClipComponent::isTrimmingActive() const {
    return isTrimming;
}

void AudioClipComponent::mouseDoubleClick(const juce::MouseEvent& event) {
    if (!onDoubleClick) {
        return;
    }
    const auto bounds = getLocalBounds().toFloat();
    const auto relative = event.getEventRelativeTo(this);
    const auto clipStart = clip.getSessionStartSample();
    const auto clipEnd = clip.getSessionEndSample();
    const auto clipLength = std::max<int64>(1, clipEnd - clipStart);
    const auto xRatio = juce::jlimit(0.0f, 1.0f, relative.position.x / std::max(1.0f, bounds.getWidth()));
    const auto sampleOffset = static_cast<int64>(std::llround(static_cast<double>(clipLength) * xRatio));
    const auto sample = clipStart + sampleOffset;
    onDoubleClick(*this, sample, event.mods.isShiftDown());
}

void AudioClipComponent::mouseMove(const juce::MouseEvent& event) {
    if (isTrimming) {
        return;
    }
    const auto localX = event.getEventRelativeTo(this).position.x;
    const auto bounds = getLocalBounds().toFloat();
    const float handleWidth = 6.0f;
    if (localX <= handleWidth || localX >= bounds.getWidth() - handleWidth) {
        setMouseCursor(juce::MouseCursor::LeftRightResizeCursor);
    } else {
        setMouseCursor(juce::MouseCursor::NormalCursor);
    }
}

void AudioClipComponent::mouseExit(const juce::MouseEvent&) {
    if (isTrimming) {
        return;
    }
    setMouseCursor(juce::MouseCursor::NormalCursor);
}

void AudioClipComponent::mouseDown(const juce::MouseEvent& event) {
    const auto localX = event.getEventRelativeTo(this).position.x;
    const auto bounds = getLocalBounds().toFloat();
    const float handleWidth = 6.0f;
    if (localX <= handleWidth) {
        activeTrimHandle = TrimHandle::Head;
    } else if (localX >= bounds.getWidth() - handleWidth) {
        activeTrimHandle = TrimHandle::Tail;
    } else {
        activeTrimHandle = TrimHandle::None;
        return;
    }
    isTrimming = true;
    setMouseCursor(juce::MouseCursor::LeftRightResizeCursor);
}

void AudioClipComponent::mouseDrag(const juce::MouseEvent& event) {
    if (!isTrimming || activeTrimHandle == TrimHandle::None) {
        return;
    }
    if (!onTrimDrag) {
        return;
    }
    const auto bounds = getLocalBounds().toFloat();
    const float width = std::max(1.0f, bounds.getWidth());
    const auto relative = event.getEventRelativeTo(this);
    const float ratio = juce::jlimit(0.0f, 1.0f, relative.position.x / width);
    const auto clipStart = clip.getSessionStartSample();
    const auto clipEnd = clip.getSessionEndSample();
    const auto clipLength = std::max<int64>(1, clipEnd - clipStart);
    const auto sampleOffset = static_cast<int64>(std::llround(static_cast<double>(clipLength) * ratio));
    const auto sample = clipStart + sampleOffset;
    setMouseCursor(juce::MouseCursor::LeftRightResizeCursor);
    onTrimDrag(clip.getId(), sample, activeTrimHandle == TrimHandle::Head, false);
}

void AudioClipComponent::mouseUp(const juce::MouseEvent& event) {
    if (!isTrimming || activeTrimHandle == TrimHandle::None) {
        return;
    }
    if (onTrimDrag) {
        const auto bounds = getLocalBounds().toFloat();
        const float width = std::max(1.0f, bounds.getWidth());
        const auto relative = event.getEventRelativeTo(this);
        const float ratio = juce::jlimit(0.0f, 1.0f, relative.position.x / width);
        const auto clipStart = clip.getSessionStartSample();
        const auto clipEnd = clip.getSessionEndSample();
        const auto clipLength = std::max<int64>(1, clipEnd - clipStart);
        const auto sampleOffset = static_cast<int64>(std::llround(static_cast<double>(clipLength) * ratio));
        const auto sample = clipStart + sampleOffset;
        onTrimDrag(clip.getId(), sample, activeTrimHandle == TrimHandle::Head, true);
    }
    isTrimming = false;
    activeTrimHandle = TrimHandle::None;
    setMouseCursor(juce::MouseCursor::NormalCursor);
}
