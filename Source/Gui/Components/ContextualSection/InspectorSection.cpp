#include "InspectorSection.h"

InspectorSection::InspectorSection() {
    setTitle("Inspecteur");
    insertsComponent.addInsert("Effet 1");
    insertsComponent.addInsert("Effet 2");
    insertsComponent.addInsert("Effet 3");
    addAndMakeVisible(insertsComponent);
    sendsComponent.addSend("Send 1");
    sendsComponent.addSend("Send 2");
    addAndMakeVisible(sendsComponent);
}

void InspectorSection::resized() {
    auto bounds = getContainerBounds();
    const int gap = 8;
    const int width = (bounds.getWidth() - gap) / 2;

    auto leftBounds = bounds.removeFromLeft(width);
    bounds.removeFromLeft(gap);
    auto rightBounds = bounds;

    insertsComponent.setBounds(leftBounds);
    sendsComponent.setBounds(rightBounds);
}
