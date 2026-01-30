#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"
#include "ControlsPanel.h"
#include "TimecodesDisplay.h"
#include "TrackFolderActivationPanel.h"
#include "AutomationPanel.h"
#include "GlobalVuMeter.h"

/// Top-level header component.
class Header : public juce::Component {
public:
    explicit Header(Edit& edit);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    Edit& edit;
    ControlsPanel controlsPanel;
    TimecodesDisplay timecodesDisplay;
    AutomationPanel automationPanel;
    TrackFolderActivationPanel trackFolderActivationPanel;
    GlobalVuMeter globalVuMeter;
};
