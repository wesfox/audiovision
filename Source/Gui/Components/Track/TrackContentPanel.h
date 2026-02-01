#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"
#include "Core/Edit/EditState.h"
#include "CursorTimeline.h"
#include "PlayheadFollowController.h"
#include "TimelineRuler.h"
#include "Gui/Utils/SelectionManager.h"

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
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseMove(const juce::MouseEvent& event) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

private:
    void handleAsyncUpdate() override;
    void timerCallback() override;
    int getTrackIndexAtY(int y) const;
    void updateSelectionRange(int hoverIndex);

    std::unique_ptr<CursorTimeline> cursorTimeline;
    std::unique_ptr<TimelineRuler> timelineRuler;
    std::unique_ptr<PlayheadFollowController> playheadController;

    Edit& edit;
    SelectionManager& selectionManager;
    std::map<String, std::shared_ptr<class TrackContent>> trackContentComponents;
};
