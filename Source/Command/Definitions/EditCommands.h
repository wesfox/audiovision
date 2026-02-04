#pragma once

#include <JuceHeader.h>

#include <functional>

#include "Core/Edit/Edit.h"

class SelectionManager;

/// Handles edit view command execution.
class EditCommands {
public:
    /// Create an edit command handler.
    /// @param edit edit owning the view state
    /// @param selectionManager selection manager for track-scoped actions
    /// @param toggleDebugWatchWindow callback to show or hide the debug watch window
    /// @param saveEdit callback to save the current project
    explicit EditCommands(Edit& edit,
                          SelectionManager& selectionManager,
                          std::function<void()> toggleDebugWatchWindow = {},
                          std::function<void()> saveEdit = {});

    void getAllCommands(juce::Array<juce::CommandID>& commands);
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result);
    bool perform(const juce::ApplicationCommandTarget::InvocationInfo& info);
    bool handlesCommand(juce::CommandID commandID) const;
    bool handleWheelCommand(juce::CommandID commandID, float delta);

private:
    void zoom(float ratio);
    void scrollView(float delta);
    void splitClipsAtCursorOrSelection();
    void healClipsAtCursorOrSelection();
    void deleteClipsInSelection();
    void toggleDebugWatchWindow();
    void undo();
    void saveEdit();

    Edit& edit;
    SelectionManager& selectionManager;

    /// TODO: remove debug watch window toggle once commands are split.
    std::function<void()> toggleDebugWatchWindowCallback;
    std::function<void()> saveEditCallback;
};
