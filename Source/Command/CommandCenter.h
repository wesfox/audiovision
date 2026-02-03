#pragma once

#include <JuceHeader.h>

#include "Command/Definitions/TransportCommands.h"
#include "Command/Definitions/EditCommands.h"
#include "Command/Definitions/WaveformCommands.h"
#include "Command/WheelCommandManager.h"

class CursorController;

/// Owns the app-wide command manager and command targets.
class CommandCenter : public juce::ApplicationCommandTarget,
                      public WheelCommandManager::Target {
public:
    /// Create the command center for a given edit.
    /// @param edit edit owning transport actions
    CommandCenter(Edit& edit, CursorController& cursorController);

    /// Access the command manager.
    juce::ApplicationCommandManager& getCommandManager();

    /// Access key mappings for key listener registration.
    juce::KeyPressMappingSet& getKeyMappings();

    void getAllCommands(juce::Array<juce::CommandID>& commands) override;
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;
    juce::ApplicationCommandTarget* getNextCommandTarget() override;
    bool handleWheelCommand(juce::CommandID commandId, float delta) override;

private:
    juce::ApplicationCommandManager commandManager;
    TransportCommands transportCommands;
    EditCommands editCommands;
    WaveformCommands waveformCommands;
};
