#include "EditCommands.h"

#include "Command/Commands.h"
#include "Core/Track/AudioTrack.h"
#include "Gui/Utils/SelectionManager.h"

#include <algorithm>
#include <unordered_set>
#include <vector>

namespace {
constexpr float kZoomStep = 0.1f;
constexpr float kScrollWheelStep = 1.0f;
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

    std::unordered_set<String> selectedIds;
    selectedIds.reserve(selectedTrackIds.size());
    for (const auto& trackId : selectedTrackIds) {
        selectedIds.insert(trackId);
    }

    for (const auto& track : edit.getTracks()) {
        if (!track) {
            continue;
        }
        const auto trackId = track->getId();
        if (selectedIds.find(trackId) == selectedIds.end()) {
            continue;
        }
        auto audioTrack = std::dynamic_pointer_cast<AudioTrack>(track);
        if (!audioTrack) {
            continue;
        }
        for (const auto splitSample : splitSamples) {
            audioTrack->splitClipsAtSample(splitSample);
        }
    }
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

    std::unordered_set<String> selectedIds;
    selectedIds.reserve(selectedTrackIds.size());
    for (const auto& trackId : selectedTrackIds) {
        selectedIds.insert(trackId);
    }

    for (const auto& track : edit.getTracks()) {
        if (!track) {
            continue;
        }
        const auto trackId = track->getId();
        if (selectedIds.find(trackId) == selectedIds.end()) {
            continue;
        }
        auto audioTrack = std::dynamic_pointer_cast<AudioTrack>(track);
        if (!audioTrack) {
            continue;
        }
        audioTrack->healClipsInRange(rangeStart, rangeEnd);
    }
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

    std::unordered_set<String> selectedIds;
    selectedIds.reserve(selectedTrackIds.size());
    for (const auto& trackId : selectedTrackIds) {
        selectedIds.insert(trackId);
    }

    for (const auto& track : edit.getTracks()) {
        if (!track) {
            continue;
        }
        const auto trackId = track->getId();
        if (selectedIds.find(trackId) == selectedIds.end()) {
            continue;
        }
        auto audioTrack = std::dynamic_pointer_cast<AudioTrack>(track);
        if (!audioTrack) {
            continue;
        }
        audioTrack->deleteClipsInRange(rangeStart, rangeEnd);
    }
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
