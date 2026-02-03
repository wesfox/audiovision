#include "CommandCenter.h"

#include "Command/Shortcuts/ShortcutMappings.h"

CommandCenter::CommandCenter(Edit& edit, CursorController& cursorController)
    : transportCommands(edit, cursorController),
      editCommands(edit),
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
    if (waveformCommands.handlesCommand(info.commandID)) {
        return waveformCommands.perform(info);
    }
    return false;
}

juce::ApplicationCommandTarget* CommandCenter::getNextCommandTarget() {
    return nullptr;
}
