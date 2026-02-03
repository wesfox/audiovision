#pragma once

#include <JuceHeader.h>

#include "Command/TrackCommandManager.h"
#include "Core/Edit/Edit.h"
#include "Core/Track/Track.h"
#include "Gui/Utils/SelectionManager.h"
#include "../Common/ui/EditableText.h"
#include "../Common/ui/SelectableList.h"
#include "HeaderTrackButtons/ArmedToggle.h"
#include "HeaderTrackButtons/InputMonitoringToggle.h"
#include "HeaderTrackButtons/MuteToggle.h"
#include "HeaderTrackButtons/SoloToggle.h"

class TrackHeader : public juce::Component,
                    private SelectionManager::Listener,
                    private juce::ValueTree::Listener {
public:
    TrackHeader(Track& track, Edit& edit, SelectionManager& selectionManager, TrackCommandManager& trackCommandManager);
    ~TrackHeader() override;

    void resized() override;

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;

    void setTrackName(const String &name) const;
private:
    void selectionChanged() override;
    void valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& property) override;
    void updateToggleStates();

    /// constructor values
    String trackId;

    String outputName = "";
    juce::Colour trackColour;
    int height = 80;

    bool isWaveformShowed = false;
    String outputTrackName;

    // track ref
    Track& track;
    Edit& edit;
    SelectionManager& selectionManager;
    TrackCommandManager& trackCommandManager;
    bool isSelected = false;
    juce::ValueTree trackStateNode;

    // subComponents
    std::unique_ptr<EditableText> trackName;
    std::unique_ptr<SelectableList> selector;
    std::unique_ptr<ArmedToggle> armedToggle;
    std::unique_ptr<InputMonitoringToggle> inputMonitoringToggle;
    std::unique_ptr<SoloToggle> soloToggle;
    std::unique_ptr<MuteToggle> activeToggle;
};
