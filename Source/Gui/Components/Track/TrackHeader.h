#pragma once

#include <JuceHeader.h>

#include "Core/Track/Track.h"
#include "Gui/Components/Common/EditableText.h"
#include "Gui/Components/Common/ToggleTextButton.h"

class TrackHeader : public juce::Component {
public:
    explicit TrackHeader(Track& track);

    void resized() override;

    void paint(juce::Graphics& g) override;

    void setTrackName(const String &name) const;
private:
    /// constructor values
    String trackId;

    /// state values
    bool isArmed = false;
    bool isInputMonitoring = false;
    bool isSolo = false;
    bool isActive = true;
    String outputName = "";
    juce::Colour trackColour;
    int height = 80;

    bool isWaveformShowed = false;
    String outputTrackName;

    // track ref
    Track& track;

    // subComponents
    std::unique_ptr<EditableText> trackName;
    std::unique_ptr<ToggleTextButton> armedToggle;
    std::unique_ptr<ToggleTextButton> inputMonitoringToggle;
    std::unique_ptr<ToggleTextButton> soloToggle;
    std::unique_ptr<ToggleTextButton> activeToggle;
};
