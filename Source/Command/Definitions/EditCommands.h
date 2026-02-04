#pragma once

#include <JuceHeader.h>

#include <functional>

#include "Core/Edit/Edit.h"

/// Handles edit view command execution.
class EditCommands {
public:
    /// Create an edit command handler.
    /// @param edit edit owning the view state
    /// @param toggleDebugWatchWindow callback to show or hide the debug watch window
    explicit EditCommands(Edit& edit, std::function<void()> toggleDebugWatchWindow = {});

    void getAllCommands(juce::Array<juce::CommandID>& commands);
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result);
    bool perform(const juce::ApplicationCommandTarget::InvocationInfo& info);
    bool handlesCommand(juce::CommandID commandID) const;
    bool handleWheelCommand(juce::CommandID commandID, float delta);

private:
    void zoom(float ratio);
    void scrollView(float delta);
    void toggleDebugWatchWindow();

    Edit& edit;

    /// TODO: remove debug watch window toggle once commands are split.
    std::function<void()> toggleDebugWatchWindowCallback;
};
