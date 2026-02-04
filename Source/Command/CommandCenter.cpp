#include "CommandCenter.h"

#include "Command/Shortcuts/ShortcutMappings.h"

CommandCenter::CommandCenter(Edit& edit,
                             CursorController& cursorController,
                             SelectionManager& selectionManager,
                             TrackCommandManager& trackCommandManager,
                             std::function<void()> toggleDebugWatchWindow,
                             std::function<void()> saveEdit)
    : transportCommands(edit, cursorController),
      editCommands(edit,
                   selectionManager,
                   std::move(toggleDebugWatchWindow),
                   std::move(saveEdit)),
      trackCommands(selectionManager, trackCommandManager),
      waveformCommands(edit) {
    commandManager.registerAllCommandsForTarget(this);
    commandManager.setFirstCommandTarget(this);
    ShortcutMappings::applyDefaultMappings(commandManager);
}

juce::ApplicationCommandManager& CommandCenter::getCommandManager() {
    return commandManager;
}

juce::KeyPressMappingSet& CommandCenter::getKeyMappings() {
    return *commandManager.getKeyMappings();
}

void CommandCenter::getAllCommands(juce::Array<juce::CommandID>& commands) {
    transportCommands.getAllCommands(commands);
    editCommands.getAllCommands(commands);
    trackCommands.getAllCommands(commands);
    waveformCommands.getAllCommands(commands);
}

void CommandCenter::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) {
    if (transportCommands.handlesCommand(commandID)) {
        transportCommands.getCommandInfo(commandID, result);
        return;
    }
    if (editCommands.handlesCommand(commandID)) {
        editCommands.getCommandInfo(commandID, result);
        return;
    }
    if (trackCommands.handlesCommand(commandID)) {
        trackCommands.getCommandInfo(commandID, result);
        return;
    }
    if (waveformCommands.handlesCommand(commandID)) {
        waveformCommands.getCommandInfo(commandID, result);
    }
}

bool CommandCenter::perform(const InvocationInfo& info) {
    if (transportCommands.handlesCommand(info.commandID)) {
        return transportCommands.perform(info);
    }
    if (editCommands.handlesCommand(info.commandID)) {
        return editCommands.perform(info);
    }
    if (trackCommands.handlesCommand(info.commandID)) {
        return trackCommands.perform(info);
    }
    if (waveformCommands.handlesCommand(info.commandID)) {
        return waveformCommands.perform(info);
    }
    return false;
}

juce::ApplicationCommandTarget* CommandCenter::getNextCommandTarget() {
    return nullptr;
}

bool CommandCenter::handleWheelCommand(juce::CommandID commandId, float delta) {
    return editCommands.handleWheelCommand(commandId, delta);
}
