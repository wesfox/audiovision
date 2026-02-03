#include "PlayheadViewFollow.h"

#include <algorithm>

PlayheadViewFollow::PlayheadViewFollow(Edit& edit)
    : edit(edit) {
}

void PlayheadViewFollow::followRightEdge(int64 playheadSample) {
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

void PlayheadViewFollow::followBothEdges(int64 playheadSample) {
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

void PlayheadViewFollow::shiftViewBy(int64 delta) {
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
