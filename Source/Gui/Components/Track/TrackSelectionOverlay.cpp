#include "TrackSelectionOverlay.h"

#include "TrackContent.h"
#include "AudioClipComponent.h"
#include "CursorTimeline.h"
#include "TimelineRuler.h"
#include "Gui/Utils/CursorController.h"
#include "Gui/Utils/ViewRangeMapper.h"

TrackSelectionOverlay::TrackSelectionOverlay(Edit& edit, SelectionManager& selectionManager)
    : edit(edit),
      selectionManager(selectionManager) {
}

bool TrackSelectionOverlay::hitTest(int x, int y) {
    const auto localPoint = juce::Point<int>(x, y);
    const auto parentPoint = localPoint + getPosition();
    auto* underlying = findUnderlyingComponent(parentPoint);
    if (underlying == nullptr) {
        return false;
    }
    return dynamic_cast<AudioClipComponent*>(underlying) == nullptr;
}

void TrackSelectionOverlay::mouseDown(const juce::MouseEvent& event) {
    if (!event.mods.isLeftButtonDown()) {
        return;
    }
    const auto parentPoint = event.getEventRelativeTo(getParentComponent()).position.toInt();
    if (findTrackContentAt(parentPoint) == nullptr) {
        return;
    }
    selectionManager.mouseDown(event, this);
    if (event.mods.isShiftDown()) {
        return;
    }
    ViewRangeMapper mapper(edit, static_cast<float>(getWidth()));
    if (!mapper.isValid()) {
        // View range and width must be valid to map samples.
        jassert(false);
        return;
    }
    const auto cursorSample = mapper.xToSample(event.position.x);
    selectionManager.getCursorController().setCursorSample(cursorSample);
}

void TrackSelectionOverlay::mouseDrag(const juce::MouseEvent& event) {
    selectionManager.mouseDrag(event, this);
}

void TrackSelectionOverlay::mouseMove(const juce::MouseEvent& event) {
    selectionManager.mouseMove(event, this);
}

void TrackSelectionOverlay::mouseEnter(const juce::MouseEvent& event) {
    selectionManager.mouseEnter(event, this);
}

void TrackSelectionOverlay::mouseUp(const juce::MouseEvent&) {
    selectionManager.mouseUp();
}

void TrackSelectionOverlay::mouseDoubleClick(const juce::MouseEvent& event) {
    const auto parentPoint = event.getEventRelativeTo(getParentComponent()).position.toInt();
    auto* trackContent = findTrackContentAt(parentPoint);
    if (trackContent == nullptr) {
        return;
    }
    ViewRangeMapper mapper(edit, static_cast<float>(getWidth()));
    if (!mapper.isValid()) {
        // View range and width must be valid to map samples.
        jassert(false);
        return;
    }
    const auto sample = mapper.xToSample(event.position.x);
    trackContent->handleDoubleClick(sample, event.mods.isShiftDown());
}

TrackContent* TrackSelectionOverlay::findTrackContentAt(juce::Point<int> parentPoint) const {
    auto* component = findUnderlyingComponent(parentPoint);
    while (component != nullptr) {
        if (auto* trackContent = dynamic_cast<TrackContent*>(component)) {
            return trackContent;
        }
        component = component->getParentComponent();
    }
    return nullptr;
}

juce::Component* TrackSelectionOverlay::findUnderlyingComponent(juce::Point<int> parentPoint) const {
    auto* parent = getParentComponent();
    if (parent == nullptr) {
        return nullptr;
    }
    for (int index = parent->getNumChildComponents() - 1; index >= 0; --index) {
        auto* child = parent->getChildComponent(index);
        if (child == this || child == nullptr || !child->isVisible()) {
            continue;
        }
        if (dynamic_cast<CursorTimeline*>(child) != nullptr
            || dynamic_cast<TimelineRuler*>(child) != nullptr) {
            continue;
        }
        if (!child->getBounds().contains(parentPoint)) {
            continue;
        }
        auto localPoint = parentPoint - child->getPosition();
        return child->getComponentAt(localPoint.x, localPoint.y);
    }
    return nullptr;
}
