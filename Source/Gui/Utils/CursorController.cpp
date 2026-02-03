#include "CursorController.h"

#include "Gui/Utils/SelectionManager.h"

CursorController::CursorController(Edit& edit, SelectionManager& selectionManager)
    : edit(edit),
      selectionManager(selectionManager),
      viewFollow(edit) {
    startTimerHz(30);
}

int64 CursorController::getCursorSample() const {
    return edit.getState().getCursorSample();
}

void CursorController::setCursorSample(int64 sample) {
    edit.getState().setCursorSample(sample);
    if (const auto transport = edit.getTransport()) {
        if (!transport->isPlaying()) {
            transport->setPlayheadSample(sample);
        } else {
            hasMovedDuringPlayback = true;
        }
    }
    // edit.getState().clearSelectionRange();
}

int64 CursorController::onPlayRequested() {
    const auto startSample = getCursorSample();
    playStartSample = startSample;
    hasMovedDuringPlayback = false;
    return startSample;
}

void CursorController::onSelectionRangeChanged(int64 startSample, int64 endSample) {
    if (const auto transport = edit.getTransport()) {
        if (transport->isPlaying()) {
            hasMovedDuringPlayback = true;
        }
    }
    if (startSample == endSample) {
        edit.getState().clearSelectionRange();
        return;
    }
    if (startSample < endSample)
        edit.getState().setSelectionRange(startSample, endSample);
    else // inverse stat and end sample if they are reversed
        edit.getState().setSelectionRange(endSample, startSample);
}

void CursorController::onSelectionCleared() {
    edit.getState().clearSelectionRange();
}

void CursorController::timerCallback() {
    const auto transport = edit.getTransport();
    if (!transport) {
        return;
    }
    const auto stopSample = transport->consumeStopSample();
    if (!stopSample.has_value()) {
        return;
    }

    // if (!edit.getState().hasSelectionRange()) {
    //     return;
    // }

    if (hasMovedDuringPlayback && !edit.getState().getInsertionFollowsPlayback()) {
        transport->setPlayheadSample(edit.getState().getCursorSample());
    }
    else {
        edit.getState().clearSelectionRange();
        if (edit.getState().getInsertionFollowsPlayback()) {
            edit.getState().setCursorSample(stopSample.value());
        }
        else {
            transport->setPlayheadSample(edit.getState().getCursorSample());
        }
    }


}

////////////////////////////////////////////////////////////////////////////////////
/// PlayHead direct setting logic, it is here because it actually               ///
/// moves the cursor                                                            ///
///////////////////////////////////////////////////////////////////////////////////

/// First, the events that triggers other functions defined belows

void CursorController::onPlaybackTick(int64 playheadSample) {
    viewFollow.followRightEdge(playheadSample);
}

void CursorController::onPointerDown(int64 previousSample, int64 playheadSample) {
    setCursorPositionFromUserAction(playheadSample);
    viewFollow.followBothEdges(playheadSample);
}

void CursorController::onPointerDrag(int64 playheadSample) {
    setCursorPositionFromUserAction(playheadSample);
    viewFollow.followBothEdges(playheadSample);
}

void CursorController::onPointerUp(int64 previousSample, int64 playheadSample, bool wasDrag) {
    if (const auto transport = edit.getTransport()) {
        if (transport->isPlaying()) {
            return;
        }
    }
    if (!wasDrag && previousSample == playheadSample) {
        return;
    }
    if (const auto transport = edit.getTransport()) {
        transport->setPlayheadSample(playheadSample);
    }
    setCursorSample(playheadSample);
    viewFollow.followBothEdges(playheadSample);
}

void CursorController::setCursorPositionFromUserAction(int64 playheadSample) {
    if (const auto transport = edit.getTransport()) {
        if (transport->isPlaying()) {
            setCursorSample(playheadSample);
        } else {
            transport->setPlayheadSample(playheadSample);
            setCursorSample(playheadSample);
        }
    }
}
