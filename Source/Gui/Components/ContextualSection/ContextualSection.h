#pragma once

#include <JuceHeader.h>

#include "SceneContextPanel.h"
#include "SendsList.h"
#include "InspectorSection.h"
#include "ContextualComponent/VolumeAndPan.h"
#include "ContextualComponent/InsertsComponent.h"

/// Hosts contextual tools in the right column.
class ContextualSection : public juce::Component {
public:
    ContextualSection();

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    SceneContextPanel sceneContextPanel;
    SendsList sendsList;
    InspectorSection inspectorSection;
    VolumeAndPan volumeAndPan;
};
