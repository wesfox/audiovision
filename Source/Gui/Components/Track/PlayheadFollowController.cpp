#include "PlayheadFollowController.h"

namespace {
class SetPlayheadAction : public juce::UndoableAction {
public:
    SetPlayheadAction(const std::weak_ptr<Transport>& transport,
                      int64 previousSample,
                      int64 nextSample)
        : transport(transport), previousSample(previousSample), nextSample(nextSample) {
    }

    bool perform() override {
        if (auto transportPtr = transport.lock()) {
            transportPtr->setCursorPosition(nextSample);
        }
        return true;
    }

    bool undo() override {
        if (auto transportPtr = transport.lock()) {
            transportPtr->setCursorPosition(previousSample);
        }
        return true;
    }

    int getSizeInUnits() override { return 1; }

private:
    std::weak_ptr<Transport> transport;
    int64 previousSample = 0;
    int64 nextSample = 0;
};
}

PlayheadFollowController::PlayheadFollowController(Edit& edit) : edit(edit) {
}

void PlayheadFollowController::onPlaybackTick(int64 cursorSample) {
    followRightEdge(cursorSample);
}

void PlayheadFollowController::onPointerDown(int64 previousSample, int64 cursorSample) {
    setTransportPosition(cursorSample);
    followBothEdges(cursorSample);
}

void PlayheadFollowController::onPointerDrag(int64 cursorSample) {
    setTransportPosition(cursorSample);
    followBothEdges(cursorSample);
}

void PlayheadFollowController::onPointerUp(int64 previousSample, int64 cursorSample, bool wasDrag) {
    if (!wasDrag && previousSample == cursorSample) {
        return;
    }
    edit.performUndoable("Playhead Move",
                         new SetPlayheadAction(edit.getTransport(), previousSample, cursorSample));
    followBothEdges(cursorSample);
}

void PlayheadFollowController::followRightEdge(int64 cursorSample) {
    const auto viewStart = edit.getViewStartSample();
    const auto viewEnd = edit.getViewEndSample();
    const auto viewLength = viewEnd - viewStart;
    if (viewLength <= 0) {
        return;
    }

    if (cursorSample > viewEnd) {
        const auto step = std::max<int64>(1, viewLength / 5);
        shiftViewBy(step);
    }
}

void PlayheadFollowController::followBothEdges(int64 cursorSample) {
    const auto viewStart = edit.getViewStartSample();
    const auto viewEnd = edit.getViewEndSample();
    const auto viewLength = viewEnd - viewStart;
    if (viewLength <= 0) {
        return;
    }

    const int64 edgeMargin = 2;
    const auto step = std::max<int64>(1, viewLength / 100);
    if (cursorSample > (viewEnd - edgeMargin)) {
        shiftViewBy(step);
    } else if (cursorSample < (viewStart + edgeMargin)) {
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

void PlayheadFollowController::setTransportPosition(int64 cursorSample) {
    if (auto transport = edit.getTransport()) {
        transport->setCursorPosition(cursorSample);
    }
}
