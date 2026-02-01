#pragma once

#include <JuceHeader.h>

#include <optional>
#include <unordered_set>

#include "Core/Edit/Edit.h"

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

    /// Replace the selection with the provided track ids.
    /// @param ids selected track ids
    void setSelection(const std::vector<String>& ids);

    /// Check whether a track id is selected.
    /// @param trackId track identifier to query
    bool isSelected(const String& trackId) const;

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

private:
    int getTrackIndexAtY(int y) const;
    void updateSelectionRange(int hoverIndex);
    void notifyListeners();

    Edit& edit;
    std::unordered_set<String> selectedIds;
    juce::ListenerList<Listener> listeners;
    int anchorTrackIndex = -1;
    int hoverTrackIndex = -1;
    bool isSelecting = false;
    std::optional<int64_t> selectionAnchorSample;
    std::optional<int64_t> selectionHoverSample;

    std::optional<int64_t> getSampleAtPosition(const juce::MouseEvent& event,
                                               juce::Component* relativeTo) const;
};
