#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"

/// Coordinates playhead following behavior for playback, clicks, and drags.
class PlayheadFollowController {
public:
    explicit PlayheadFollowController(Edit& edit);

    /// Called while transport is playing to keep the playhead visible.
    /// @param playheadSample current transport sample position
    void onPlaybackTick(int64 playheadSample);

    /// Called when the user begins dragging the playhead.
    /// @param previousSample playhead position before the pointer down
    /// @param playheadSample target transport sample position
    void onPointerDown(int64 previousSample, int64 playheadSample);

    /// Called while the user drags the playhead.
    /// @param playheadSample target transport sample position
    void onPointerDrag(int64 playheadSample);

    /// Called when the user finishes dragging the playhead.
    /// @param previousSample playhead position before the gesture
    /// @param playheadSample final transport sample position
    /// @param wasDrag true when the gesture included a drag
    void onPointerUp(int64 previousSample, int64 playheadSample, bool wasDrag);

private:
    void followRightEdge(int64 playheadSample);
    void followBothEdges(int64 playheadSample);
    void shiftViewBy(int64 delta);
    void setTransportPosition(int64 playheadSample);

    Edit& edit;
};
