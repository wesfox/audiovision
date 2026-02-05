#pragma once

#include <JuceHeader.h>

#include <optional>

#include "Core/Edit/Edit.h"
#include "Gui/Utils/PlayheadViewFollow.h"

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

    /// Apply selection range changes.
    /// @param startSample selection start sample
    /// @param endSample selection end sample
    void onSelectionRangeChanged(int64 startSample, int64 endSample);

    /// Apply selection clear updates.
    void onSelectionCleared();

    /// Follow the playhead during playback.
    /// @param playheadSample playhead sample to follow
    void onPlaybackTick(int64 playheadSample);

    /// Begin cursor interaction at a playhead sample.
    /// @param previousSample sample before interaction
    /// @param playheadSample target playhead sample
    void onPointerDown(int64 previousSample, int64 playheadSample);

    /// Continue cursor interaction at a playhead sample.
    /// @param playheadSample target playhead sample
    void onPointerDrag(int64 playheadSample);

    /// End cursor interaction at a playhead sample.
    /// @param previousSample sample before interaction
    /// @param playheadSample target playhead sample
    /// @param wasDrag true when drag occurred
    void onPointerUp(int64 previousSample, int64 playheadSample, bool wasDrag);

    /// Ensure the cursor is visible in the view range.
    /// @param cursorSample cursor sample to reveal
    void ensureCursorVisible(int64 cursorSample);

private:
    void timerCallback() override;
    bool shouldFollowPlayhead() const;
    void setCursorPositionFromUserAction(int64 playheadSample);

    Edit& edit;
    SelectionManager& selectionManager;
    PlayheadViewFollow viewFollow;
    std::optional<int64_t> playStartSample;
    bool hasMovedDuringPlayback = false;
};
