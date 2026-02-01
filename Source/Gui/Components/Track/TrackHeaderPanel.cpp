#include "TrackHeaderPanel.h"

#include "TrackHeader.h"
#include "Gui/Style/Font.h"

// ------------------------ MainComponent Implementation ------------------------

TrackHeaderPanel::TrackHeaderPanel(Edit& edit, SelectionManager& selectionManager)
    : edit(edit),
      selectionManager(selectionManager),
      tracks(edit.getTracks()),
      trackHeaderComponents(std::map<String, std::shared_ptr<TrackHeader> >()) {
    for (const auto& track: tracks) {
        auto trackHeader = std::make_shared<TrackHeader>(*track, selectionManager);
        addAndMakeVisible(*trackHeader);
        trackHeaderComponents.emplace(track->getId(), std::move(trackHeader));
    }
}

void TrackHeaderPanel::resized() {
    auto bounds = getLocalBounds().removeFromLeft(265);
    const auto rulerHeight = edit.getState().getTimelineHeight();
    bounds.removeFromTop(rulerHeight);
    for (const auto& track : tracks) {
        const auto trackHeader = trackHeaderComponents.at(track->getId());
        trackHeader->setBounds(bounds.removeFromTop(static_cast<int>(track->getHeight())));
    }
}

void TrackHeaderPanel::paint(juce::Graphics &g) {
    g.setColour(juce::Colour(0xFFF6F4FF));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 6.0f);

    const auto rulerHeight = edit.getState().getTimelineHeight();
    auto headerBounds = getLocalBounds().removeFromLeft(265).removeFromTop(rulerHeight);
    g.setColour(juce::Colour(0xFF2F2C3F));
    g.setFont(Fonts::h3());
    g.drawText("Ruler", headerBounds.reduced(8, 0).translated(0, -2),
               juce::Justification::centredLeft, false);

    g.setColour(juce::Colour(0xFFC8A5FF));
    const auto y = static_cast<float>(rulerHeight - 1);
    g.drawLine(0.0f, y, static_cast<float>(getWidth()), y, 1.0f);
}
