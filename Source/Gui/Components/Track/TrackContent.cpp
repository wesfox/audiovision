#include "TrackContent.h"

#include <cmath>
#include <algorithm>

#include "AudioClipComponent.h"
#include "Core/Track/AudioTrack.h"

// ------------------------ MainComponent Implementation ------------------------

TrackContent::TrackContent(const Edit& edit, std::shared_ptr<Track> track)
    : edit(edit), track(std::move(track)) {
    if (auto audioTrack = std::dynamic_pointer_cast<AudioTrack>(this->track)) {
        for (const auto& clip : audioTrack->getAudioClips()) {
            if (!clip) {
                continue;
            }
            auto clipComponent = std::make_unique<AudioClipComponent>(*clip, this->track->getColour());
            addAndMakeVisible(clipComponent.get());
            clipComponents.push_back(std::move(clipComponent));
        }
    }
}

void TrackContent::resized() {
    updateLayout();
}

void TrackContent::updateLayout() {
    if (!track) {
        return;
    }

    const auto viewStart = edit.getViewStartSample();
    const auto viewEnd = edit.getViewEndSample();
    const auto viewLength = viewEnd - viewStart;
    if (viewLength <= 0) {
        return;
    }

    const auto bounds = getLocalBounds();
    const float width = static_cast<float>(bounds.getWidth());
    const float height = static_cast<float>(bounds.getHeight());

    for (auto& clipComponent : clipComponents) {
        const auto& clip = clipComponent->getClip();
        const auto clipStart = clip.getSessionStartSample();
        const auto clipEnd = clip.getSessionEndSample();
        if (clipEnd <= viewStart || clipStart >= viewEnd) {
            clipComponent->setVisible(false);
            continue;
        }

        // const auto clippedStart = std::max(clipStart, viewStart);
        // const auto clippedEnd = std::min(clipEnd, viewEnd);
        const float startX = (static_cast<float>(clipStart - viewStart) / static_cast<float>(viewLength)) * width;
        const float endX = (static_cast<float>(clipEnd - viewStart) / static_cast<float>(viewLength)) * width;
        const int clipX = static_cast<int>(std::floor(startX));
        const int clipWidth = std::max(1, static_cast<int>(std::ceil(endX - startX)));
        clipComponent->setBounds(clipX, 0, clipWidth, static_cast<int>(height));
        clipComponent->setVisible(true);
    }
}

void TrackContent::paint(juce::Graphics& g) {
    g.setColour(juce::Colour::fromRGBA(0, 0, 0, 80));
    const auto b = getLocalBounds().toFloat();
    g.drawLine(b.getX(), b.getBottom() + 0.5f, b.getWidth(), b.getBottom() + 0.5f, 2.0f );
}
