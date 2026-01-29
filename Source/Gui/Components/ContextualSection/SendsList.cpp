#include "SendsList.h"

SendsList::SendsList()
    : PrimaryContainer(nullptr),
      addSendButton("+ add send") {
    setTitle("Sends");
    addAndMakeVisible(reverbA);
    addAndMakeVisible(reverbB);
    addAndMakeVisible(addSendButton);

    reverbA.addSend("Altiverb 8");
    reverbB.addSend("Altiverb 9");
}

void SendsList::resized() {
    auto bounds = getContainerBounds().reduced(4);
    const int gap = 4;
    const int width = (bounds.getWidth() - gap) / 2;
    const int footerHeight = 30;

    auto footerBounds = bounds.removeFromBottom(footerHeight);
    auto leftBounds = bounds.removeFromLeft(width);
    bounds.removeFromLeft(gap);
    auto rightBounds = bounds;

    reverbA.setBounds(leftBounds);
    reverbB.setBounds(rightBounds);

    auto buttonBounds = footerBounds.withSizeKeepingCentre(70, footerBounds.getHeight()).reduced(0,4);
    addSendButton.setBounds(buttonBounds);
}
