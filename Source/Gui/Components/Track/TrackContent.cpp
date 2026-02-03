#include "TrackContent.h"

#include <cmath>
#include <algorithm>

#include "AudioClipComponent.h"
#include "Core/Track/AudioTrack.h"
#include "Gui/Utils/ViewRangeMapper.h"

// ------------------------ MainComponent Implementation ------------------------

TrackContent::TrackContent(Edit& edit, SelectionManager& selectionManager, std::shared_ptr<Track> track)
    : edit(edit),
      selectionManager(selectionManager),
      track(std::move(track)) {
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
    edit.getState().getRoot().addListener(this);
    selectionManager.addListener(this);
    if (this->track != nullptr) {
        isSelected = selectionManager.isSelected(this->track->getId());
    }
    applyWaveformScale();
}

TrackContent::~TrackContent() {
    edit.getState().getRoot().removeListener(this);
    selectionManager.removeListener(this);
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
        clipComponent->setViewRange(viewStart, viewEnd);
        clipComponent->setVisible(true);
    }
}

void TrackContent::paint(juce::Graphics& g) {
    if (isSelected) {
        g.setColour(juce::Colour(0x114C2C7E));
        g.fillRect(getLocalBounds());
    }
    g.setColour(juce::Colour::fromRGBA(0, 0, 0, 80));
    const auto b = getLocalBounds().toFloat();
    g.drawLine(b.getX(), b.getBottom() + 0.5f, b.getWidth(), b.getBottom() + 0.5f, 2.0f );

}

void TrackContent::paintOverChildren(juce::Graphics& g) {
    if (!isSelected) {
        return;
    }
    const auto mapper = getMapper();

    const bool hasSelectionRange = edit.getState().hasSelectionRange();
    bool hasSelection = false;
    if (hasSelectionRange) {
        const auto selectionRange = selectionManager.getSelectionRangeSamples();
        if (!selectionRange.has_value()) {
            return;
        }
        const auto rangeStart = std::min(selectionRange->first, selectionRange->second);
        const auto rangeEnd = std::max(selectionRange->first, selectionRange->second);
        const auto [clampedStart, clampedEnd] = mapper.clampRangeToView(rangeStart, rangeEnd);
        if (clampedEnd > clampedStart) {
            hasSelection = true;
            const float startX = mapper.sampleToX(clampedStart);
            const float endX = mapper.sampleToX(clampedEnd);
            const auto x = std::min(startX, endX);
            const auto width = std::max(1.0f, std::abs(endX - startX));
            g.setColour(juce::Colour::fromString("#99000000"));
            g.fillRect(x, 0.0f, width, static_cast<float>(getHeight()));
        }
    }

    const auto transport = edit.getTransport();
    if (!transport) {
        return;
    }
    if (hasSelectionRange && hasSelection) {
        return;
    }
    const auto cursorSample = edit.getState().getCursorSample();
    if (cursorSample < mapper.getViewStartSample() || cursorSample > mapper.getViewEndSample()) {
        return;
    }

    // draw tick
    const auto tick = juce::Time::getMillisecondCounter() / 350;
    if ((tick % 2) == 0) {
        return;
    }

    const float x = mapper.sampleToX(cursorSample);
    g.setColour(juce::Colour::fromString("#FF998893"));
    g.drawLine(x, 0.0f, x, static_cast<float>(getHeight()), 1.0f);
}

void TrackContent::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property) {
    if (property != EditState::kWaveformScaleId) {
        return;
    }
    applyWaveformScale();
}

void TrackContent::applyWaveformScale() {
    const float waveformScale = edit.getState().getWaveformScale();
    for (auto& clipComponent : clipComponents) {
        clipComponent->setWaveformScale(waveformScale);
    }
}

void TrackContent::selectionChanged() {
    if (track == nullptr) {
        return;
    }
    const bool nextSelected = selectionManager.isSelected(track->getId());
    if (nextSelected == isSelected) {
        return;
    }
    isSelected = nextSelected;
    repaint();
}

ViewRangeMapper TrackContent::getMapper() const {
    ViewRangeMapper mapper(edit, static_cast<float>(getWidth()));
    if (!mapper.isValid()) {
        // View range and width must be valid to map samples.
        jassert(false);
    }
    return mapper;
}
