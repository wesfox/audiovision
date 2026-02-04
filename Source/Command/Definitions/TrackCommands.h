#pragma once

#include <JuceHeader.h>

class SelectionManager;
class TrackCommandManager;

/// Handles track selection command execution.
class TrackCommands {
public:
    /// Create track command handlers.
    /// @param selectionManager selection manager for track ids
    /// @param trackCommandManager track command manager to execute
    TrackCommands(SelectionManager& selectionManager, TrackCommandManager& trackCommandManager);

    void getAllCommands(juce::Array<juce::CommandID>& commands);
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result);
    bool perform(const juce::ApplicationCommandTarget::InvocationInfo& info);
    bool handlesCommand(juce::CommandID commandID) const;

private:
    void toggleSoloSelection();
    void toggleMuteSelection();
    void clearAllSolo();

    SelectionManager& selectionManager;
    TrackCommandManager& trackCommandManager;
};
