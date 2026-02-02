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
        }
    }
    edit.getState().clearSelectionRange();
}

int64 CursorController::onPlayRequested() {
    const auto startSample = getCursorSample();
    playStartSample = startSample;
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

}

void CursorController::onSelectionRangeChanged(int64 startSample, int64 endSample) {
    edit.getState().setSelectionRange(startSample, endSample);
}

void CursorController::onSelectionCleared() {
    edit.getState().clearSelectionRange();
}

void CursorController::timerCallback() {
    const auto transport = edit.getTransport();
    if (!transport) {
        return;
    }
    if (!transport->consumeEndReached()) {
        return;
    }
    if (!edit.getState().hasSelectionRange()) {
        return;
    }
    if (hasMovedDuringPlayback) {
        transport->setPlayheadSample(edit.getState().getCursorSample());
    }
    else {
        if (edit.getState().getInsertionFollowsPlayback()) {
            edit.getState().setCursorSample(transport->getPlayheadSample());
        }
        else {
            transport->setPlayheadSample(edit.getState().getCursorSample());
        }
    }


}
