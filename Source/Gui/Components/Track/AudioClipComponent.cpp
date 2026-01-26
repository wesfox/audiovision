#include "AudioClipComponent.h"

#include "Core/AudioClip/AudioClip.h"

AudioClipComponent::AudioClipComponent(const AudioClip& clip, const juce::Colour colour) : clip(clip), colour(colour) {
}

const AudioClip& AudioClipComponent::getClip() const {
    return clip;
}

void AudioClipComponent::paint(juce::Graphics& g) {

    const auto waveform = clip.getWaveformData();
    if (waveform.size() < 2) {
        return;
    }

    auto b = getLocalBounds().reduced(0,4);
    auto bounds = b.toFloat();
    auto fill = juce::Colour(0xCCC8D9B8);
    g.setColour(fill);
    g.fillRoundedRectangle(bounds,8.0f);

    g.setColour(juce::Colour(0xFF63A129));
    g.drawRoundedRectangle(bounds,8.0f,2.0f);

    const float midY = bounds.getCentreY();
    const float halfHeight = bounds.getHeight() * 0.45f;
    const float width = bounds.getWidth();
    const size_t count = waveform.size();

    g.setColour(juce::Colour(0xFF63A129));
    g.drawLine(bounds.getX(), midY, bounds.getWidth(), midY, 1.0f);
    for (size_t i = 0; i < count; ++i) {
        const float x = bounds.getX() + (static_cast<float>(i) / static_cast<float>(count - 1)) * width;
        const float amp = juce::jlimit(0.0f, 1.0f, waveform[i]) * halfHeight * 2;
        g.drawLine(x, midY - amp, x, midY + amp, 1.0f);
    }
    // clip name
    g.setFont(10.0f);
    g.setColour(juce::Colours::darkgrey);
    const auto titleBounds = getLocalBounds().reduced(6,6).removeFromLeft(100).removeFromTop(15);
    g.drawText("Clip Name", titleBounds, Justification::topLeft, true);
}
