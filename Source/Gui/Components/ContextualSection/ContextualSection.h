#pragma once

#include <JuceHeader.h>

#include "SceneContextPanel.h"
#include "SendsList/SendsList.h"
#include "InspectorSection/InspectorSection.h"
#include "InspectorSection/InsertsComponent.h"

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
};
