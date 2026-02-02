#include "PlayheadFollowController.h"

namespace {
class SetPlayheadAction : public juce::UndoableAction {
public:
    SetPlayheadAction(EditState& editState,
                      const std::weak_ptr<Transport>& transport,
                      int64 previousSample,
                      int64 nextSample)
        : editState(editState),
          transport(transport),
          previousSample(previousSample),
          nextSample(nextSample) {
    }

    bool perform() override {
        if (auto transportPtr = transport.lock()) {
            transportPtr->setPlayheadSample(nextSample);
            if (!transportPtr->isPlaying()) {
                editState.setCursorSample(nextSample);
            }
        }
        return true;
    }

    bool undo() override {
        if (auto transportPtr = transport.lock()) {
            transportPtr->setPlayheadSample(previousSample);
            if (!transportPtr->isPlaying()) {
                editState.setCursorSample(previousSample);
            }
        }
        return true;
    }

    int getSizeInUnits() override { return 1; }

private:
    EditState& editState;
    std::weak_ptr<Transport> transport;
    int64 previousSample = 0;
    int64 nextSample = 0;
};
}

PlayheadFollowController::PlayheadFollowController(Edit& edit) : edit(edit) {
}

void PlayheadFollowController::onPlaybackTick(int64 playheadSample) {
    followRightEdge(playheadSample);
}

void PlayheadFollowController::onPointerDown(int64 previousSample, int64 playheadSample) {
    setTransportPosition(playheadSample);
    followBothEdges(playheadSample);
}

void PlayheadFollowController::onPointerDrag(int64 playheadSample) {
    setTransportPosition(playheadSample);
    followBothEdges(playheadSample);
}

void PlayheadFollowController::onPointerUp(int64 previousSample, int64 playheadSample, bool wasDrag) {
    if (auto transport = edit.getTransport()) {
        if (transport->isPlaying()) {
            return;
        }
    }
    if (!wasDrag && previousSample == playheadSample) {
        return;
    }
    edit.performUndoable("Playhead Move",
                         new SetPlayheadAction(edit.getState(), edit.getTransport(), previousSample, playheadSample));
    followBothEdges(playheadSample);
}

void PlayheadFollowController::followRightEdge(int64 playheadSample) {
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

void PlayheadFollowController::followBothEdges(int64 playheadSample) {
    const auto viewStart = edit.getViewStartSample();
    const auto viewEnd = edit.getViewEndSample();
    const auto viewLength = viewEnd - viewStart;
    if (viewLength <= 0) {
        return;
    }

    const int64 edgeMargin = 2;
    const auto step = std::max<int64>(1, viewLength / 100);
    if (playheadSample > (viewEnd - edgeMargin)) {
        shiftViewBy(step);
    } else if (playheadSample < (viewStart + edgeMargin)) {
        shiftViewBy(-step);
    }
}

void PlayheadFollowController::shiftViewBy(int64 delta) {
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

void PlayheadFollowController::setTransportPosition(int64 playheadSample) {
    if (auto transport = edit.getTransport()) {
        if (transport->isPlaying()) {
            edit.getState().setCursorSample(playheadSample);
        } else {
            transport->setPlayheadSample(playheadSample);
            edit.getState().setCursorSample(playheadSample);
        }
    }
}
