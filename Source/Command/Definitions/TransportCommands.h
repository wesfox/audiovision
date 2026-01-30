#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"

/// Handles transport command execution.
class TransportCommands {
public:
    /// Create a transport command target.
    /// @param edit edit owning the transport
    explicit TransportCommands(Edit& edit);

    void getAllCommands(juce::Array<juce::CommandID>& commands);
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result);
    bool perform(const juce::ApplicationCommandTarget::InvocationInfo& info);
    bool handlesCommand(juce::CommandID commandID) const;

private:
    void togglePlayPause();

    Edit& edit;
};
