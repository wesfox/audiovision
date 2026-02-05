#pragma once

#include <JuceHeader.h>

#include <optional>
#include <unordered_set>
#include <vector>

#include "Core/Edit/Edit.h"
#include "Gui/Utils/ViewRangeMapper.h"

class CursorController;

/// Tracks selection state for timeline tracks.
class SelectionManager {
public:
    /// Notifies listeners about selection changes.
    class Listener {
    public:
        virtual ~Listener() = default;
        /// Called when selection changes.
        virtual void selectionChanged() = 0;
    };

    /// Create a selection manager bound to the edit.
    /// @param edit edit containing track order and timeline height
    explicit SelectionManager(Edit& edit);

    /// Bind the cursor controller.
    /// @param controller cursor controller to notify
    void setCursorController(CursorController* controller);

    /// Access the cursor controller.
    CursorController& getCursorController();

    /// Replace the selection with the provided track ids.
    /// @param ids selected track ids
    void setSelection(const std::vector<String>& ids);

    /// Check whether a track id is selected.
    /// @param trackId track identifier to query
    bool isSelected(const String& trackId) const;

    /// Read selected track ids in edit order.
    std::vector<String> getSelectedTrackIds() const;

    /// Register a selection listener.
    /// @param listener listener to add
    void addListener(Listener* listener);

    /// Remove a selection listener.
    /// @param listener listener to remove
    void removeListener(Listener* listener);

    /// Read the anchor sample for a selection range.
    /// @return anchor sample when selection started
    std::optional<int64_t> getSelectionAnchorSample() const;

    /// Read the sample range defined by the selection anchor and hover.
    /// @return selection sample range when available
    std::optional<std::pair<int64_t, int64_t>> getSelectionRangeSamples() const;

    /// True when a selection drag is active.
    bool isSelectingRange() const;

    /// Collapse the selection range to a single sample.
    /// @param sample sample to keep as the selection point
    void collapseSelectionToSample(int64 sample);

    /// Update the selection range from a command.
    /// @param anchorSample anchor sample for the selection
    /// @param hoverSample hover sample for the selection
    void setSelectionRangeFromCommand(int64 anchorSample, int64 hoverSample);

    /// Handle selection on mouse down.
    /// @param event mouse event to process
    /// @param relativeTo component used for relative coordinates
    void mouseDown(const juce::MouseEvent& event, juce::Component* relativeTo);

    /// Handle selection on mouse drag.
    /// @param event mouse event to process
    /// @param relativeTo component used for relative coordinates
    void mouseDrag(const juce::MouseEvent& event, juce::Component* relativeTo);

    /// Handle selection on mouse move.
    /// @param event mouse event to process
    /// @param relativeTo component used for relative coordinates
    void mouseMove(const juce::MouseEvent& event, juce::Component* relativeTo);

    /// Handle selection when the mouse enters a component.
    /// @param event mouse event to process
    /// @param relativeTo component used for relative coordinates
    void mouseEnter(const juce::MouseEvent& event, juce::Component* relativeTo);

    /// End selection interaction.
    void mouseUp();

    /// Add a track to the current selection.
    /// @param trackId track to add
    void addTrackToSelection(const String& trackId);

    /// Toggle a track in the current selection.
    /// @param trackId track to toggle
    void toggleTrackSelection(const String& trackId);

    /// Extend the current selection range to the track.
    /// @param trackId track to extend selection to
    void extendSelectionToTrack(const String& trackId);

private:
    int getTrackIndexAtY(int y) const;
    void updateSelectionRange(int hoverIndex);
    void applySelectionRange(int startIndex, int endIndex);
    void updateSelectionSamples();
    void applySelectionSamples(int64 anchorSample, int64 hoverSample);
    void notifyListeners();

    Edit& edit;
    CursorController* cursorController = nullptr;
    std::unordered_set<String> selectedIds;
    juce::ListenerList<Listener> listeners;
    int anchorTrackIndex = -1;
    int hoverTrackIndex = -1;
    bool isSelecting = false;
    std::optional<int64_t> selectionAnchorSample;
    std::optional<int64_t> selectionHoverSample;
    int selectionComponentWidth = 0;
    bool forceSelectionCommit = false;
    int lastAnchorTrackIndex = -1;
    std::optional<int64_t> lastAnchorSample;

    std::optional<int64_t> getSampleAtPosition(const juce::MouseEvent& event,
                                               juce::Component* relativeTo) const;
    ViewRangeMapper getMapperForComponent(const juce::Component& component) const;
};
