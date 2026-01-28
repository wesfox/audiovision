#include "TrackFolderActivationPanel.h"

TrackFolderActivationPanel::TrackFolderActivationPanel(const Edit* edit)
    : SecondaryContainer(edit),
      dial("Dial", juce::Colour::fromString("#CCC8D9B8")),
      foley("Foley", juce::Colour::fromString("#AAB8D0D9")),
      amb("Amb", juce::Colour::fromString("#BAC8B8D9")),
      fx("Fx", juce::Colour::fromString("#CED9C9B8")),
      mus("Mus", juce::Colour::fromString("#A0B4B1E2")) {
    dial.setActive(true);
    foley.setActive(true);
    amb.setActive(true);
    fx.setActive(true);
    mus.setActive(true);

    addAndMakeVisible(dial);
    addAndMakeVisible(foley);
    addAndMakeVisible(amb);
    addAndMakeVisible(fx);
    addAndMakeVisible(mus);
}

void TrackFolderActivationPanel::resized() {
    auto bounds = getContainerBounds().reduced(12, 12);

    juce::Grid grid;
    grid.templateRows = { 1_fr, 1_fr };
    grid.templateColumns = { 1_fr, 1_fr, 1_fr };
    grid.setGap(juce::Grid::Px(12));

    grid.items.add({},
                    juce::GridItem(dial),
                   juce::GridItem(foley),
                   juce::GridItem(amb),
                   juce::GridItem(fx),
                   juce::GridItem(mus));

    grid.performLayout(bounds);
}
