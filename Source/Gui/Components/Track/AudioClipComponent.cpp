#include "AudioClipComponent.h"

#include "Components/Track/AudioClip/WaveformPaintStrategy.h"
#include "Core/AudioClip/AudioClip.h"

AudioClipComponent::AudioClipComponent(const AudioClip& clip, const juce::Colour colour)
    : clip(clip),
      colour(colour),
      paintStrategy(std::make_unique<WaveformPaintStrategy>()) {
    if (auto audioFile = clip.getAudioFile()) {
        thumbnail = audioFile->getThumbnail();
        if (thumbnail != nullptr) {
            thumbnail->addChangeListener(this);
        }
    }
}

AudioClipComponent::~AudioClipComponent() {
    if (thumbnail != nullptr) {
        thumbnail->removeChangeListener(this);
    }
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

void AudioClipComponent::changeListenerCallback(juce::ChangeBroadcaster*) {
    repaint();
}
