#pragma once

#include <JuceHeader.h>

#include <optional>

#include "Core/Edit/Edit.h"

class SelectionManager;

/// Coordinates cursor state and selection policy.
class CursorController : private juce::Timer {
public:
    /// Create a controller bound to the edit and selection manager.
    /// @param edit edit owning the cursor state
    /// @param selectionManager selection manager for range updates
    CursorController(Edit& edit, SelectionManager& selectionManager);

    /// Current cursor sample.
    int64 getCursorSample() const;

    /// Set the cursor sample.
    /// @param sample new cursor sample
    void setCursorSample(int64 sample);

    /// Decide the play start sample.
    int64 onPlayRequested();

    /// Apply cursor policy when playback stops.
    /// @param playheadSample playhead sample at stop
    void onStop(int64 playheadSample);

    /// Apply selection range changes.
    /// @param startSample selection start sample
    /// @param endSample selection end sample
    void onSelectionRangeChanged(int64 startSample, int64 endSample);

    /// Apply selection clear updates.
    void onSelectionCleared();

private:
    void timerCallback() override;

    Edit& edit;
    SelectionManager& selectionManager;
    std::optional<int64_t> playStartSample;
    bool hasMovedDuringPlayback = false;
};
