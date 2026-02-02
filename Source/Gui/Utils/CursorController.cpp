#include "CursorController.h"

#include "Gui/Utils/SelectionManager.h"

CursorController::CursorController(Edit& edit, SelectionManager& selectionManager)
    : edit(edit),
      selectionManager(selectionManager) {
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

void CursorController::onStop(int64 playheadSample) {
    const auto cursorPosition = getCursorSample();
    if(cursorPosition == playStartSample) {
        if (edit.getState().getInsertionFollowsPlayback()) {
            setCursorSample(playheadSample);
        } else {
            if (const auto transport = edit.getTransport()) {
                transport->setPlayheadSample(getCursorSample());
            }
        }
    }
    edit.getState().clearSelectionRange();
}

void CursorController::onSelectionRangeChanged(int64 startSample, int64 endSample) {
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
    if (!transport->consumeHasStopped()) {
        return;
    }

    // if (!edit.getState().hasSelectionRange()) {
    //     return;
    // }

    if (hasMovedDuringPlayback && edit.getState().getInsertionFollowsPlayback()) {
        transport->setPlayheadSample(edit.getState().getCursorSample());
    }
    else {
        edit.getState().clearSelectionRange();
        if (edit.getState().getInsertionFollowsPlayback()) {
            edit.getState().setCursorSample(transport->getPlayheadSample());
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
    followRightEdge(playheadSample);
}

void CursorController::onPointerDown(int64 previousSample, int64 playheadSample) {
    setTransportPosition(playheadSample);
    followBothEdges(playheadSample);
}

void CursorController::onPointerDrag(int64 playheadSample) {
    setTransportPosition(playheadSample);
    followBothEdges(playheadSample);
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
    followBothEdges(playheadSample);
}

/// Definition of the functions used upper

void CursorController::followRightEdge(int64 playheadSample) {
    const auto viewStart = edit.getViewStartSample();
    const auto viewEnd = edit.getViewEndSample();
    const auto viewLength = viewEnd - viewStart;
    if (viewLength <= 0) {
        return;
    }

    if (playheadSample < viewStart || playheadSample > viewEnd) {
        const auto leftInset = static_cast<int64>(viewLength * 0.1);
        auto newStart = playheadSample - leftInset;
        if (newStart < 0) {
            newStart = 0;
        }
        edit.getState().setViewRange(newStart, newStart + viewLength, nullptr);
        return;
    }

    if (playheadSample > viewEnd) {
        const auto step = std::max<int64>(1, viewLength / 5);
        shiftViewBy(step);
    }
}

void CursorController::followBothEdges(int64 playheadSample) {
    const auto viewStart = edit.getViewStartSample();
    const auto viewEnd = edit.getViewEndSample();
    const auto viewLength = viewEnd - viewStart;
    if (viewLength <= 0) {
        return;
    }

    constexpr int64 edgeMargin = 2;
    const auto step = std::max<int64>(1, viewLength / 100);
    if (playheadSample > (viewEnd - edgeMargin)) {
        shiftViewBy(step);
    } else if (playheadSample < (viewStart + edgeMargin)) {
        shiftViewBy(-step);
    }
}

void CursorController::shiftViewBy(int64 delta) {
    const auto viewStart = edit.getViewStartSample();
    const auto viewEnd = edit.getViewEndSample();
    const auto viewLength = viewEnd - viewStart;
    if (viewLength <= 0) {
        return;
    }

    auto newStart = viewStart + delta;
    auto newEnd = viewEnd + delta;
    if (newStart < 0) {
        newStart = 0;
        newEnd = viewLength;
    }

    edit.getState().setViewRange(newStart, newEnd, nullptr);
}

void CursorController::setTransportPosition(int64 playheadSample) {
    if (const auto transport = edit.getTransport()) {
        if (transport->isPlaying()) {
            setCursorSample(playheadSample);
        } else {
            transport->setPlayheadSample(playheadSample);
            setCursorSample(playheadSample);
        }
    }
}
