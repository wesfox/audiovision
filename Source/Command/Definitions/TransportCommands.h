#pragma once

#include <JuceHeader.h>

#include <optional>

#include "Core/Edit/Edit.h"

class CursorController;

/// Handles transport command execution.
class TransportCommands {
public:
    /// Create a transport command target.
    /// @param edit edit owning the transport
    TransportCommands(Edit& edit, CursorController& cursorController);

    void getAllCommands(juce::Array<juce::CommandID>& commands);
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result);
    bool perform(const juce::ApplicationCommandTarget::InvocationInfo& info);
    bool handlesCommand(juce::CommandID commandID) const;

private:
    void togglePlayPause();
    void toggleInsertionFollowsPlayback();

    Edit& edit;
    CursorController& cursorController;
    std::optional<int64_t> playSelectionStartSample;
};
