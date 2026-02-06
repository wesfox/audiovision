#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"
#include "Gui/Utils/SelectionManager.h"

class TrackContent;

/// Handles selection gestures over track content.
class TrackSelectionOverlay : public juce::Component {
public:
    TrackSelectionOverlay(Edit& edit, SelectionManager& selectionManager);

    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseMove(const juce::MouseEvent& event) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseDoubleClick(const juce::MouseEvent& event) override;
    bool hitTest(int x, int y) override;

private:
    TrackContent* findTrackContentAt(juce::Point<int> parentPoint) const;
    juce::Component* findUnderlyingComponent(juce::Point<int> parentPoint) const;

    Edit& edit;
    SelectionManager& selectionManager;
};
