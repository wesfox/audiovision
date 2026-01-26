#include "TrackHeaderPanel.h"

#include "TrackHeader.h"

// ------------------------ MainComponent Implementation ------------------------

TrackHeaderPanel::TrackHeaderPanel(Edit& edit) : tracks(edit.getTracks()), trackHeaderComponents(std::map<String, std::shared_ptr<TrackHeader> >()) {
    for (const auto& track: tracks) {
        auto trackHeader = std::make_shared<TrackHeader>(*track);
        addAndMakeVisible(*trackHeader);
        trackHeaderComponents.emplace(track->getId(), std::move(trackHeader));
    }
}

void TrackHeaderPanel::resized() {
    auto bounds = getLocalBounds().removeFromLeft(265);
    for (const auto& track : tracks) {
        const auto trackHeader = trackHeaderComponents.at(track->getId());
        trackHeader->setBounds(bounds.removeFromTop(static_cast<int>(track->getHeight())));
    }
}

void TrackHeaderPanel::paint(juce::Graphics &g) {
    g.setColour(juce::Colours::white);
    g.fillRect(getLocalBounds());
}
