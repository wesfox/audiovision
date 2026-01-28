#include "SendsList.h"

SendsList::SendsList()
    : PrimaryContainer(nullptr) {
    setTitle("Sends");
    addAndMakeVisible(reverbA);
    addAndMakeVisible(reverbB);

    reverbA.addSend("Altiverb 8");
    reverbB.addSend("Altiverb 9");
}

void SendsList::resized() {
    auto bounds = getContainerBounds().reduced(4);
    const int gap = 4;
    const int width = (bounds.getWidth() - gap) / 2;

    auto leftBounds = bounds.removeFromLeft(width);
    bounds.removeFromLeft(gap);
    auto rightBounds = bounds;

    reverbA.setBounds(leftBounds);
    reverbB.setBounds(rightBounds);
}
