#pragma once

#include <JuceHeader.h>

#include "SceneContextPanel.h"
#include "Gui/Components/Common/SendsList.h"

/// Hosts contextual tools in the right column.
class ContextualSection : public juce::Component {
public:
    ContextualSection();

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    SceneContextPanel sceneContextPanel;
    SendsList sendsList;
};
