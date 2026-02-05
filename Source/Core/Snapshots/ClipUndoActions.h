#pragma once

#include <JuceHeader.h>

#include <vector>

#include "Core/Snapshots/ClipSnapshot.h"

class Edit;

/// Undoable action for splitting clips.
class SplitClipsAction final : public juce::UndoableAction {
public:
    /// Create a split action.
    /// @param edit edit owning the tracks
    /// @param trackIds track ids to modify
    /// @param splitSamples split samples in session time
    SplitClipsAction(Edit& edit,
                     std::vector<String> trackIds,
                     std::vector<int64> splitSamples);

    bool perform() override;
    bool undo() override;

private:
    Edit& edit;
    std::vector<String> trackIds;
    std::vector<int64> splitSamples;
    std::vector<ClipSnapshot> beforeSnapshots;
    std::vector<ClipSnapshot> afterSnapshots;
    bool hasSnapshots = false;
};

/// Undoable action for healing clips.
class HealClipsAction final : public juce::UndoableAction {
public:
    /// Create a heal action.
    /// @param edit edit owning the tracks
    /// @param trackIds track ids to modify
    /// @param rangeStart selection start in session samples
    /// @param rangeEnd selection end in session samples
    HealClipsAction(Edit& edit,
                    std::vector<String> trackIds,
                    int64 rangeStart,
                    int64 rangeEnd);

    bool perform() override;
    bool undo() override;

private:
    Edit& edit;
    std::vector<String> trackIds;
    int64 rangeStart = 0;
    int64 rangeEnd = 0;
    std::vector<ClipSnapshot> beforeSnapshots;
    std::vector<ClipSnapshot> afterSnapshots;
    bool hasSnapshots = false;
};

/// Undoable action for deleting clips in a range.
class DeleteClipsAction final : public juce::UndoableAction {
public:
    /// Create a delete action.
    /// @param edit edit owning the tracks
    /// @param trackIds track ids to modify
    /// @param rangeStart selection start in session samples
    /// @param rangeEnd selection end in session samples
    DeleteClipsAction(Edit& edit,
                      std::vector<String> trackIds,
                      int64 rangeStart,
                      int64 rangeEnd);

    bool perform() override;
    bool undo() override;

private:
    Edit& edit;
    std::vector<String> trackIds;
    int64 rangeStart = 0;
    int64 rangeEnd = 0;
    std::vector<ClipSnapshot> beforeSnapshots;
    std::vector<ClipSnapshot> afterSnapshots;
    bool hasSnapshots = false;
};

/// Undoable action for trimming clips to a range.
class TrimToSelectionAction final : public juce::UndoableAction {
public:
    /// Create a trim action.
    /// @param edit edit owning the tracks
    /// @param trackIds track ids to modify
    /// @param rangeStart selection start in session samples
    /// @param rangeEnd selection end in session samples
    TrimToSelectionAction(Edit& edit,
                          std::vector<String> trackIds,
                          int64 rangeStart,
                          int64 rangeEnd);

    bool perform() override;
    bool undo() override;

private:
    Edit& edit;
    std::vector<String> trackIds;
    int64 rangeStart = 0;
    int64 rangeEnd = 0;
    std::vector<ClipSnapshot> beforeSnapshots;
    std::vector<ClipSnapshot> afterSnapshots;
    bool hasSnapshots = false;
};
