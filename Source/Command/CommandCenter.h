#pragma once

#include <JuceHeader.h>

#include <functional>

#include "Command/Definitions/TransportCommands.h"
#include "Command/Definitions/EditCommands.h"
#include "Command/Definitions/TrackCommands.h"
#include "Command/Definitions/WaveformCommands.h"
#include "Command/WheelCommandManager.h"

class CursorController;
class SelectionManager;
class TrackCommandManager;

/// Owns the app-wide command manager and command targets.
class CommandCenter : public juce::ApplicationCommandTarget,
                      public WheelCommandManager::Target {
public:
    /// Create the command center for a given edit.
    /// @param edit edit owning transport actions
    /// @param selectionManager selection manager for track commands
    /// @param trackCommandManager track command manager to execute
    /// @param toggleDebugWatchWindow callback to show or hide the debug watch window
    CommandCenter(Edit& edit,
                  CursorController& cursorController,
                  SelectionManager& selectionManager,
                  TrackCommandManager& trackCommandManager,
                  std::function<void()> toggleDebugWatchWindow);

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
    TrackCommands trackCommands;
    WaveformCommands waveformCommands;
};
