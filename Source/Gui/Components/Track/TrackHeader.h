#pragma once

#include <JuceHeader.h>

#include "Core/Track/Track.h"
#include "Gui/Utils/SelectionManager.h"
#include "../Common/ui/EditableText.h"
#include "../Common/ui/SelectableList.h"
#include "HeaderTrackButtons/ArmedToggle.h"
#include "HeaderTrackButtons/InputMonitoringToggle.h"
#include "HeaderTrackButtons/MuteToggle.h"
#include "HeaderTrackButtons/SoloToggle.h"

class TrackHeader : public juce::Component,
                    private SelectionManager::Listener {
public:
    TrackHeader(Track& track, SelectionManager& selectionManager);
    ~TrackHeader() override;

    void resized() override;

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;

    void setTrackName(const String &name) const;
private:
    void selectionChanged() override;

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
    SelectionManager& selectionManager;
    bool isSelected = false;

    // subComponents
    std::unique_ptr<EditableText> trackName;
    std::unique_ptr<SelectableList> selector;
    std::unique_ptr<ArmedToggle> armedToggle;
    std::unique_ptr<InputMonitoringToggle> inputMonitoringToggle;
    std::unique_ptr<SoloToggle> soloToggle;
    std::unique_ptr<MuteToggle> activeToggle;
};
