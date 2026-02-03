#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"

/// Keeps the view range aligned with the playhead.
class PlayheadViewFollow {
public:
    /// Bind to an edit for view range updates.
    /// @param edit edit owning the view range
    explicit PlayheadViewFollow(Edit& edit);

    /// Shift the view when the playhead crosses the right edge.
    /// @param playheadSample current playhead sample
    void followRightEdge(int64 playheadSample);

    /// Shift the view when the playhead crosses either edge.
    /// @param playheadSample current playhead sample
    void followBothEdges(int64 playheadSample);

private:
    void shiftViewBy(int64 delta);

    Edit& edit;
};
