#include "GlobalVuMeter.h"

GlobalVuMeter::GlobalVuMeter(const Edit* edit)
    : SecondaryContainer(edit) {
    addAndMakeVisible(vuMeter1);
    addAndMakeVisible(vuMeter2);
    addAndMakeVisible(vuMeter3);
    addAndMakeVisible(vuMeter4);
    addAndMakeVisible(vuMeter5);
    addAndMakeVisible(vuMeter6);
}

void GlobalVuMeter::resized() {
    auto bounds = getLocalBounds().reduced(4, 4);

    juce::Grid grid;
    grid.templateRows = { 1_fr };
    grid.templateColumns = { 1_fr, 1_fr, 1_fr, 1_fr, 1_fr, 1_fr };
    grid.setGap(juce::Grid::Px(2));

    grid.items.add(juce::GridItem(vuMeter1),
                   juce::GridItem(vuMeter2),
                   juce::GridItem(vuMeter3),
                   juce::GridItem(vuMeter4),
                   juce::GridItem(vuMeter5),
                   juce::GridItem(vuMeter6));

    grid.performLayout(bounds);
}
