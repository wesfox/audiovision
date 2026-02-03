#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"

/// Handles edit view command execution.
class EditCommands {
public:
    /// Create an edit command handler.
    /// @param edit edit owning the view state
    explicit EditCommands(Edit& edit);

    void getAllCommands(juce::Array<juce::CommandID>& commands);
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result);
    bool perform(const juce::ApplicationCommandTarget::InvocationInfo& info);
    bool handlesCommand(juce::CommandID commandID) const;
    bool handleWheelCommand(juce::CommandID commandID, float delta);

private:
    void zoom(float ratio);
    void scrollView(float delta);

    Edit& edit;
};
