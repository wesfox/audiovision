#include "EditCommands.h"

#include "Command/Commands.h"
#include "Core/Snapshots/ClipUndoActions.h"
#include "Core/Track/AudioTrack.h"
#include "Gui/Utils/CursorController.h"
#include "Gui/Utils/SelectionManager.h"

#include <algorithm>
#include <unordered_set>
#include <vector>

namespace {
constexpr float kZoomStep = 0.1f;
constexpr float kScrollWheelStep = 1.0f;

int64 getTimelineEndSample(const Edit& edit) {
    int64 timelineEndSample = 0;
    for (const auto& track : edit.getTracks()) {
        if (!track || !track->isAudioTrack()) {
            continue;
        }
        auto audioTrack = std::dynamic_pointer_cast<AudioTrack>(track);
        if (!audioTrack) {
            continue;
        }
        for (const auto& clip : audioTrack->getAudioClips()) {
            if (!clip) {
                continue;
            }
            timelineEndSample = std::max(timelineEndSample, clip->getSessionEndSample());
        }
    }
    return timelineEndSample;
}

std::shared_ptr<AudioTrack> findAudioTrack(Edit& edit, const String& trackId) {
    for (const auto& track : edit.getTracks()) {
        if (!track || !track->isAudioTrack()) {
            continue;
        }
        if (track->getId() == trackId) {
            return std::dynamic_pointer_cast<AudioTrack>(track);
        }
    }
    return nullptr;
}

std::optional<int64> findPreviousCutOnTrack(const AudioTrack& track, int64 sample, int64 sessionEnd) {
    std::optional<int64> previousCut;
    if (sample > 0) {
        previousCut = 0;
    }
    if (sessionEnd < sample) {
        if (!previousCut.has_value() || sessionEnd > previousCut.value()) {
            previousCut = sessionEnd;
        }
    }
    for (const auto& clip : track.getAudioClips()) {
        if (!clip) {
            continue;
        }
        const auto clipStart = clip->getSessionStartSample();
        const auto clipEnd = clip->getSessionEndSample();
        if (clipStart < sample && (!previousCut.has_value() || clipStart > previousCut.value())) {
            previousCut = clipStart;
        }
        if (clipEnd < sample && (!previousCut.has_value() || clipEnd > previousCut.value())) {
            previousCut = clipEnd;
        }
    }
    return previousCut;
}

std::optional<int64> findNextCutOnTrack(const AudioTrack& track, int64 sample, int64 sessionEnd) {
    std::optional<int64> nextCut;
    if (sessionEnd > sample) {
        nextCut = sessionEnd;
    }
    for (const auto& clip : track.getAudioClips()) {
        if (!clip) {
            continue;
        }
        const auto clipStart = clip->getSessionStartSample();
        const auto clipEnd = clip->getSessionEndSample();
        if (clipStart > sample && (!nextCut.has_value() || clipStart < nextCut.value())) {
            nextCut = clipStart;
        }
        if (clipEnd > sample && (!nextCut.has_value() || clipEnd < nextCut.value())) {
            nextCut = clipEnd;
        }
    }
    return nextCut;
}
}

EditCommands::EditCommands(Edit& edit,
                           SelectionManager& selectionManager,
                           std::function<void()> toggleDebugWatchWindow,
                           std::function<void()> saveEdit)
    : edit(edit),
      selectionManager(selectionManager),
      toggleDebugWatchWindowCallback(std::move(toggleDebugWatchWindow)),
      saveEditCallback(std::move(saveEdit)) {
}

void EditCommands::getAllCommands(juce::Array<juce::CommandID>& commands) {
    commands.add(CommandIds::EditView::zoomIn);
    commands.add(CommandIds::EditView::zoomOut);
    commands.add(CommandIds::EditView::scrollView);
    commands.add(CommandIds::EditView::toggleDebugWatchWindow);
    commands.add(CommandIds::EditView::splitClipsAtCursorOrSelection);
    commands.add(CommandIds::EditView::healClip);
    commands.add(CommandIds::EditView::deleteClipsInSelection);
    commands.add(CommandIds::EditView::undo);
    commands.add(CommandIds::EditView::trimToSelection);
    commands.add(CommandIds::EditView::tabToNextCut);
    commands.add(CommandIds::EditView::tabToPreviousCut);
    commands.add(CommandIds::EditView::moveCursorToSessionStart);
    commands.add(CommandIds::EditView::moveCursorToSessionEnd);
    commands.add(CommandIds::Project::save);
}

void EditCommands::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) {
    if (commandID == CommandIds::EditView::zoomIn) {
        result.setInfo("Zoom In", "Zoom into the timeline", "Edit", 0);
        return;
    }
    if (commandID == CommandIds::EditView::zoomOut) {
        result.setInfo("Zoom Out", "Zoom out of the timeline", "Edit", 0);
        return;
    }
    if (commandID == CommandIds::EditView::scrollView) {
        result.setInfo("Scroll View", "Scroll the timeline view", "Edit", 0);
        return;
    }
    if (commandID == CommandIds::EditView::toggleDebugWatchWindow) {
        result.setInfo("Toggle Debug Watch", "Show or hide the debug watch window", "Edit", 0);
        return;
    }
    if (commandID == CommandIds::EditView::splitClipsAtCursorOrSelection) {
        result.setInfo("Split Clips", "Split clips at the cursor or selection range", "Edit", 0);
        return;
    }
    if (commandID == CommandIds::EditView::healClip) {
        result.setInfo("Heal Clip", "Merge clips at the cursor or selection range", "Edit", 0);
        return;
    }
    if (commandID == CommandIds::EditView::deleteClipsInSelection) {
        result.setInfo("Delete Clips", "Delete clips in the selection range", "Edit", 0);
        return;
    }
    if (commandID == CommandIds::EditView::undo) {
        result.setInfo("Undo", "Undo the last action", "Edit", 0);
        return;
    }
    if (commandID == CommandIds::EditView::trimToSelection) {
        result.setInfo("Trim To Selection", "Trim clips to the selection range", "Edit", 0);
        return;
    }
    if (commandID == CommandIds::EditView::tabToNextCut) {
        result.setInfo("Tab To Next Cut", "Move cursor to the next cut", "Edit", 0);
        return;
    }
    if (commandID == CommandIds::EditView::tabToPreviousCut) {
        result.setInfo("Tab To Previous Cut", "Move cursor to the previous cut", "Edit", 0);
        return;
    }
    if (commandID == CommandIds::EditView::moveCursorToSessionStart) {
        result.setInfo("Go To Start", "Move cursor to the start of the session", "Edit", 0);
        return;
    }
    if (commandID == CommandIds::EditView::moveCursorToSessionEnd) {
        result.setInfo("Go To End", "Move cursor to the end of the session", "Edit", 0);
        return;
    }
    if (commandID == CommandIds::Project::save) {
        result.setInfo("Save", "Save the current project", "Project", 0);
    }
}

bool EditCommands::perform(const juce::ApplicationCommandTarget::InvocationInfo& info) {
    if (info.commandID == CommandIds::EditView::zoomIn) {
        zoom(-kZoomStep);
        return true;
    }
    if (info.commandID == CommandIds::EditView::zoomOut) {
        zoom(kZoomStep);
        return true;
    }
    if (info.commandID == CommandIds::EditView::scrollView) {
        return false;
    }
    if (info.commandID == CommandIds::EditView::toggleDebugWatchWindow) {
        toggleDebugWatchWindow();
        return true;
    }
    if (info.commandID == CommandIds::EditView::splitClipsAtCursorOrSelection) {
        splitClipsAtCursorOrSelection();
        return true;
    }
    if (info.commandID == CommandIds::EditView::healClip) {
        healClipsAtCursorOrSelection();
        return true;
    }
    if (info.commandID == CommandIds::EditView::deleteClipsInSelection) {
        deleteClipsInSelection();
        return true;
    }
    if (info.commandID == CommandIds::EditView::undo) {
        undo();
        return true;
    }
    if (info.commandID == CommandIds::EditView::trimToSelection) {
        trimToSelection();
        return true;
    }
    if (info.commandID == CommandIds::EditView::tabToNextCut) {
        const bool extendSelection = info.keyPress.getModifiers().isShiftDown();
        tabToNextCut(extendSelection);
        return true;
    }
    if (info.commandID == CommandIds::EditView::tabToPreviousCut) {
        const bool extendSelection = info.keyPress.getModifiers().isShiftDown();
        tabToPreviousCut(extendSelection);
        return true;
    }
    if (info.commandID == CommandIds::EditView::moveCursorToSessionStart) {
        const bool extendSelection = info.keyPress.getModifiers().isShiftDown();
        moveCursorToSessionStart(extendSelection);
        return true;
    }
    if (info.commandID == CommandIds::EditView::moveCursorToSessionEnd) {
        const bool extendSelection = info.keyPress.getModifiers().isShiftDown();
        moveCursorToSessionEnd(extendSelection);
        return true;
    }
    if (info.commandID == CommandIds::Project::save) {
        saveEdit();
        return true;
    }
    return false;
}

bool EditCommands::handlesCommand(juce::CommandID commandID) const {
    return commandID == CommandIds::EditView::zoomIn
        || commandID == CommandIds::EditView::zoomOut
        || commandID == CommandIds::EditView::scrollView
        || commandID == CommandIds::EditView::toggleDebugWatchWindow
        || commandID == CommandIds::EditView::splitClipsAtCursorOrSelection
        || commandID == CommandIds::EditView::healClip
        || commandID == CommandIds::EditView::deleteClipsInSelection
        || commandID == CommandIds::EditView::undo
        || commandID == CommandIds::EditView::trimToSelection
        || commandID == CommandIds::EditView::tabToNextCut
        || commandID == CommandIds::EditView::tabToPreviousCut
        || commandID == CommandIds::EditView::moveCursorToSessionStart
        || commandID == CommandIds::EditView::moveCursorToSessionEnd
        || commandID == CommandIds::Project::save;
}

void EditCommands::zoom(float ratio) {
    edit.zoom(ratio);
}

bool EditCommands::handleWheelCommand(juce::CommandID commandID, float delta) {
    if (commandID == CommandIds::EditView::scrollView) {
        scrollView(delta);
        return true;
    }
    return false;
}

void EditCommands::scrollView(float delta) {
    const auto viewStart = edit.getViewStartSample();
    const auto viewEnd = edit.getViewEndSample();
    const auto viewLength = viewEnd - viewStart;
    if (viewLength <= 0) {
        // View length must be positive to scroll.
        jassert(false);
        return;
    }

    const auto offset = static_cast<int64>(std::llround(static_cast<double>(viewLength)
                                                        * kScrollWheelStep
                                                        * static_cast<double>(delta)));
    auto newStart = viewStart + offset;
    auto newEnd = viewEnd + offset;
    if (newStart < 0) {
        newStart = 0;
        newEnd = viewLength;
    }
    edit.getState().setViewRange(newStart, newEnd, nullptr);
}

void EditCommands::splitClipsAtCursorOrSelection() {
    const auto selectedTrackIds = selectionManager.getSelectedTrackIds();
    if (selectedTrackIds.empty()) {
        return;
    }

    std::vector<int64> splitSamples;
    const auto& state = edit.getState();
    if (state.hasSelectionRange()) {
        auto startSample = state.getSelectionStartSample();
        auto endSample = state.getSelectionEndSample();
        if (startSample > endSample) {
            std::swap(startSample, endSample);
        }
        splitSamples.push_back(startSample);
        if (endSample != startSample) {
            splitSamples.push_back(endSample);
        }
    } else {
        splitSamples.push_back(state.getCursorSample());
    }

    if (splitSamples.empty()) {
        return;
    }

    edit.performUndoable("Split Clips",
                         new SplitClipsAction(edit, selectedTrackIds, splitSamples));
}

void EditCommands::healClipsAtCursorOrSelection() {
    const auto selectedTrackIds = selectionManager.getSelectedTrackIds();
    if (selectedTrackIds.empty()) {
        return;
    }

    int64 rangeStart = 0;
    int64 rangeEnd = 0;
    const auto& state = edit.getState();
    if (state.hasSelectionRange()) {
        rangeStart = state.getSelectionStartSample();
        rangeEnd = state.getSelectionEndSample();
        if (rangeStart > rangeEnd) {
            std::swap(rangeStart, rangeEnd);
        }
    } else {
        rangeStart = state.getCursorSample();
        rangeEnd = rangeStart;
    }

    edit.performUndoable("Heal Clips",
                         new HealClipsAction(edit, selectedTrackIds, rangeStart, rangeEnd));
}

void EditCommands::deleteClipsInSelection() {
    const auto& state = edit.getState();
    if (!state.hasSelectionRange()) {
        return;
    }

    auto rangeStart = state.getSelectionStartSample();
    auto rangeEnd = state.getSelectionEndSample();
    if (rangeStart > rangeEnd) {
        std::swap(rangeStart, rangeEnd);
    }

    const auto selectedTrackIds = selectionManager.getSelectedTrackIds();
    if (selectedTrackIds.empty()) {
        return;
    }

    edit.performUndoable("Delete Clips",
                         new DeleteClipsAction(edit, selectedTrackIds, rangeStart, rangeEnd));
    edit.getState().clearSelectionRange();
}

void EditCommands::trimToSelection() {
    const auto& state = edit.getState();
    if (!state.hasSelectionRange()) {
        return;
    }

    auto rangeStart = state.getSelectionStartSample();
    auto rangeEnd = state.getSelectionEndSample();
    if (rangeStart > rangeEnd) {
        std::swap(rangeStart, rangeEnd);
    }

    const auto selectedTrackIds = selectionManager.getSelectedTrackIds();
    if (selectedTrackIds.empty()) {
        return;
    }

    edit.performUndoable("Trim To Selection",
                         new TrimToSelectionAction(edit, selectedTrackIds, rangeStart, rangeEnd));
}

void EditCommands::toggleDebugWatchWindow() {
    if (!toggleDebugWatchWindowCallback) {
        // Debug watch toggle callback must be configured before use.
        jassert(false);
        return;
    }
    toggleDebugWatchWindowCallback();
}

void EditCommands::undo() {
    edit.getUndoManager().undo();
}

void EditCommands::saveEdit() {
    if (!saveEditCallback) {
        // Save callback must be configured before use.
        jassert(false);
        return;
    }
    saveEditCallback();
}

void EditCommands::tabToNextCut(bool extendSelection) {
    const auto selectedTrackIds = selectionManager.getSelectedTrackIds();
    if (selectedTrackIds.empty()) {
        return;
    }

    std::unordered_set<String> selectedSet;
    selectedSet.reserve(selectedTrackIds.size());
    for (const auto& id : selectedTrackIds) {
        selectedSet.insert(id);
    }

    auto cursorSample = edit.getState().getCursorSample();
    if (extendSelection) {
        if (const auto range = selectionManager.getSelectionRangeSamples()) {
            cursorSample = range->second;
        }
    }
    std::optional<int64> nextCut;
    for (const auto& track : edit.getTracks()) {
        if (!track || !track->isAudioTrack()) {
            continue;
        }
        if (selectedSet.find(track->getId()) == selectedSet.end()) {
            continue;
        }
        auto audioTrack = std::dynamic_pointer_cast<AudioTrack>(track);
        if (!audioTrack) {
            continue;
        }
        for (const auto& clip : audioTrack->getAudioClips()) {
            if (!clip) {
                continue;
            }
            const auto clipStart = clip->getSessionStartSample();
            const auto clipEnd = clip->getSessionEndSample();
            if (clipStart > cursorSample && (!nextCut.has_value() || clipStart < nextCut.value())) {
                nextCut = clipStart;
            }
            if (clipEnd > cursorSample && (!nextCut.has_value() || clipEnd < nextCut.value())) {
                nextCut = clipEnd;
            }
        }
    }
    const auto timelineEndSample = getTimelineEndSample(edit);
    if (timelineEndSample > cursorSample
        && (!nextCut.has_value() || timelineEndSample < nextCut.value())) {
        nextCut = timelineEndSample;
    }

    if (!nextCut.has_value()) {
        return;
    }

    auto& cursorController = selectionManager.getCursorController();
    if (extendSelection) {
        int64 anchorSample = cursorSample;
        if (const auto range = selectionManager.getSelectionRangeSamples()) {
            anchorSample = range->first;
        }
        const auto cursorTarget = std::min(anchorSample, nextCut.value());
        selectionManager.setSelectionRangeFromCommand(anchorSample, nextCut.value());
        cursorController.ensureCursorVisible(cursorTarget);
    } else {
        cursorController.setCursorSample(nextCut.value());
        cursorController.ensureCursorVisible(nextCut.value());
        selectionManager.collapseSelectionToSample(nextCut.value());
    }
}

void EditCommands::tabToPreviousCut(bool extendSelection) {
    const auto selectedTrackIds = selectionManager.getSelectedTrackIds();
    if (selectedTrackIds.empty()) {
        return;
    }

    std::unordered_set<String> selectedSet;
    selectedSet.reserve(selectedTrackIds.size());
    for (const auto& id : selectedTrackIds) {
        selectedSet.insert(id);
    }

    auto cursorSample = edit.getState().getCursorSample();
    if (extendSelection) {
        if (const auto range = selectionManager.getSelectionRangeSamples()) {
            cursorSample = range->first;
        }
    }
    std::optional<int64> previousCut;
    for (const auto& track : edit.getTracks()) {
        if (!track || !track->isAudioTrack()) {
            continue;
        }
        if (selectedSet.find(track->getId()) == selectedSet.end()) {
            continue;
        }
        auto audioTrack = std::dynamic_pointer_cast<AudioTrack>(track);
        if (!audioTrack) {
            continue;
        }
        for (const auto& clip : audioTrack->getAudioClips()) {
            if (!clip) {
                continue;
            }
            const auto clipStart = clip->getSessionStartSample();
            const auto clipEnd = clip->getSessionEndSample();
            if (clipStart < cursorSample && (!previousCut.has_value() || clipStart > previousCut.value())) {
                previousCut = clipStart;
            }
            if (clipEnd < cursorSample && (!previousCut.has_value() || clipEnd > previousCut.value())) {
                previousCut = clipEnd;
            }
        }
    }
    if (cursorSample > 0 && (!previousCut.has_value() || 0 > previousCut.value())) {
        previousCut = 0;
    }
    const auto timelineEndSample = getTimelineEndSample(edit);
    if (timelineEndSample < cursorSample
        && (!previousCut.has_value() || timelineEndSample > previousCut.value())) {
        previousCut = timelineEndSample;
    }

    if (!previousCut.has_value()) {
        return;
    }

    auto& cursorController = selectionManager.getCursorController();
    if (extendSelection) {
        int64 anchorSample = cursorSample;
        if (const auto range = selectionManager.getSelectionRangeSamples()) {
            anchorSample = range->second;
        }
        const auto cursorTarget = std::min(anchorSample, previousCut.value());
        selectionManager.setSelectionRangeFromCommand(anchorSample, previousCut.value());
        cursorController.ensureCursorVisible(cursorTarget);
    } else {
        cursorController.setCursorSample(previousCut.value());
        cursorController.ensureCursorVisible(previousCut.value());
        selectionManager.collapseSelectionToSample(previousCut.value());
    }
}

void EditCommands::moveCursorToSessionStart(bool extendSelection) {
    const int64 sessionStartSample = 0;
    const auto cursorSample = edit.getState().getCursorSample();
    auto& cursorController = selectionManager.getCursorController();
    if (extendSelection) {
        selectionManager.setSelectionRangeFromCommand(cursorSample, sessionStartSample);
        cursorController.ensureCursorVisible(std::min(cursorSample, sessionStartSample));
        return;
    }
    cursorController.ensureCursorVisible(sessionStartSample);
    if (!extendSelection) {
        selectionManager.collapseSelectionToSample(sessionStartSample);
    }
}

void EditCommands::moveCursorToSessionEnd(bool extendSelection) {
    const auto sessionEndSample = getTimelineEndSample(edit);
    const auto cursorSample = edit.getState().getCursorSample();
    auto& cursorController = selectionManager.getCursorController();
    if (extendSelection) {
        selectionManager.setSelectionRangeFromCommand(cursorSample, sessionEndSample);
        cursorController.ensureCursorVisible(std::min(cursorSample, sessionEndSample));
        return;
    }
    cursorController.ensureCursorVisible(sessionEndSample);
    if (!extendSelection) {
        selectionManager.collapseSelectionToSample(sessionEndSample);
    }
}

void selectRegionBetweenCuts(Edit& edit,
                             SelectionManager& selectionManager,
                             const String& trackId,
                             int64 sample) {
    const auto audioTrack = findAudioTrack(edit, trackId);
    if (!audioTrack) {
        return;
    }

    int64 leftCut = 0;
    int64 rightCut = getTimelineEndSample(edit);
    for (const auto& clip : audioTrack->getAudioClips()) {
        if (!clip) {
            continue;
        }
        const auto clipStart = clip->getSessionStartSample();
        const auto clipEnd = clip->getSessionEndSample();
        if (clipStart < sample) {
            leftCut = std::max(leftCut, clipStart);
        }
        if (clipEnd < sample) {
            leftCut = std::max(leftCut, clipEnd);
        }
        if (clipStart > sample) {
            rightCut = std::min(rightCut, clipStart);
        }
        if (clipEnd > sample) {
            rightCut = std::min(rightCut, clipEnd);
        }
    }

    selectionManager.setSelectionRangeFromCommand(leftCut, rightCut);
    selectionManager.getCursorController().ensureCursorVisible(leftCut);
}

void extendSelectionToNearestCut(Edit& edit,
                                 SelectionManager& selectionManager,
                                 const String& trackId,
                                 int64 sample) {
    const auto range = selectionManager.getSelectionRangeSamples();
    if (!range.has_value()) {
        return;
    }

    const auto audioTrack = findAudioTrack(edit, trackId);
    if (!audioTrack) {
        return;
    }

    const auto selectionStart = std::min(range->first, range->second);
    const auto selectionEnd = std::max(range->first, range->second);
    const auto sessionEnd = getTimelineEndSample(edit);
    const auto leftCut = findPreviousCutOnTrack(*audioTrack, sample, sessionEnd).value_or(0);
    const auto rightCut = findNextCutOnTrack(*audioTrack, sample, sessionEnd).value_or(sessionEnd);
    const auto mergedStart = std::min(selectionStart, leftCut);
    const auto mergedEnd = std::max(selectionEnd, rightCut);
    selectionManager.setSelectionRangeFromCommand(mergedStart, mergedEnd);
    selectionManager.getCursorController().ensureCursorVisible(mergedStart);
}
