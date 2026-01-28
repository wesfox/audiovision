#include "ContextualSection.h"

ContextualSection::ContextualSection() {
    addAndMakeVisible(sceneContextPanel);
    addAndMakeVisible(sendsList);
}

void ContextualSection::paint(juce::Graphics& g) {
    juce::ignoreUnused(g);
}

void ContextualSection::resized() {
    auto bounds = getLocalBounds();
    sceneContextPanel.setBounds(bounds.removeFromTop(SceneContextPanel::kHeight));
    bounds.removeFromTop(8);
    sendsList.setBounds(bounds.removeFromTop(SendsList::kHeight));
}
