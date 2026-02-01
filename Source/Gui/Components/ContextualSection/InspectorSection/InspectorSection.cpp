#include "InspectorSection.h"

InspectorSection::InspectorSection() {
    setTitle("Inspecteur");
    insertsComponent.addInsert("Effet 1");
    insertsComponent.addInsert("Effet 2");
    addAndMakeVisible(insertsComponent);
    sendsComponent.addSend("Send 1");
    sendsComponent.addSend("Send 2");
    addAndMakeVisible(sendsComponent);
    addAndMakeVisible(volumeAndPan);
    addAndMakeVisible(clipVersion);
}

void InspectorSection::resized() {
    auto bounds = getContainerBounds();
    const int gap = 8;
    const int clipHeight = 70;
    const int availableHeight = bounds.getHeight() - clipHeight - gap * 3;
    const int rowHeight = std::max(VolumeAndPan::kMinHeight, availableHeight / 3);

    auto topRow = bounds.removeFromTop(rowHeight);
    const int splitGap = 4;
    const int splitWidth = (topRow.getWidth() - splitGap) / 2;
    auto leftBounds = topRow.removeFromLeft(splitWidth);
    topRow.removeFromLeft(splitGap);
    auto rightBounds = topRow;

    insertsComponent.setBounds(leftBounds);
    sendsComponent.setBounds(rightBounds);

    bounds.removeFromTop(gap);
    volumeAndPan.setBounds(bounds.removeFromTop(rowHeight));
    bounds.removeFromTop(gap);
    clipVersion.setBounds(bounds.removeFromTop(std::min(clipHeight,bounds.getHeight())));
}
