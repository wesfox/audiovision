#include "ClipVersion.h"

ClipVersion::ClipVersion() {
    setTitle("Clip version");
    versionList.setItems({ { "Current Version Name", 1 } });
    versionList.getComboBox().setSelectedId(1, juce::dontSendNotification);
    addAndMakeVisible(versionList);
}

void ClipVersion::resized() {
    auto bounds = getContainerBounds().reduced(8, 8);
    auto listBounds = bounds.removeFromTop(40);
    versionList.setBounds(listBounds);
}
