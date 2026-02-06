#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"
#include "Core/Edit/EditState.h"
#include "CursorTimeline.h"
#include "TimelineRuler.h"
#include "TrackSelectionOverlay.h"
#include "Gui/Utils/SelectionManager.h"
#include "Gui/Utils/ViewRangeMapper.h"

class TrackContentPanel : public juce::Component,
                          public juce::ValueTree::Listener,
                          private juce::AsyncUpdater,
                          private juce::Timer {
public:
    TrackContentPanel(Edit& edit, SelectionManager& selectionManager);
    ~TrackContentPanel() override;
    void updateLayout();
    void resized() override;
    void paint(juce::Graphics& g) override;
    void valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& property) override;

private:
    void handleAsyncUpdate() override;
    void timerCallback() override;
    ViewRangeMapper getMapper(float width) const;
    void updateSelectionRange(int hoverIndex);

    std::unique_ptr<CursorTimeline> cursorTimeline;
    std::unique_ptr<TimelineRuler> timelineRuler;
    std::unique_ptr<TrackSelectionOverlay> selectionOverlay;

    Edit& edit;
    SelectionManager& selectionManager;
    std::map<String, std::shared_ptr<class TrackContent>> trackContentComponents;
};
