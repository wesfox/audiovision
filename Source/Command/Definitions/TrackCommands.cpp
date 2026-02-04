#include "TrackCommands.h"

#include "Command/Commands.h"
#include "Command/TrackCommandManager.h"
#include "Gui/Utils/SelectionManager.h"

TrackCommands::TrackCommands(SelectionManager& selectionManager, TrackCommandManager& trackCommandManager)
    : selectionManager(selectionManager),
      trackCommandManager(trackCommandManager) {
}

void TrackCommands::getAllCommands(juce::Array<juce::CommandID>& commands) {
    commands.add(CommandIds::Track::toggleSoloSelection);
    commands.add(CommandIds::Track::clearAllSolo);
    commands.add(CommandIds::Track::toggleMuteSelection);
}

void TrackCommands::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) {
    if (commandID == CommandIds::Track::toggleSoloSelection) {
        result.setInfo("Toggle Solo Selection", "Toggle solo for the selected tracks", "Track", 0);
        return;
    }
    if (commandID == CommandIds::Track::clearAllSolo) {
        result.setInfo("Clear All Solo", "Clear solo state for all tracks", "Track", 0);
        return;
    }
    if (commandID == CommandIds::Track::toggleMuteSelection) {
        result.setInfo("Toggle Mute Selection", "Toggle mute for the selected tracks", "Track", 0);
    }
}

bool TrackCommands::perform(const juce::ApplicationCommandTarget::InvocationInfo& info) {
    if (info.commandID == CommandIds::Track::toggleSoloSelection) {
        toggleSoloSelection();
        return true;
    }
    if (info.commandID == CommandIds::Track::clearAllSolo) {
        clearAllSolo();
        return true;
    }
    if (info.commandID == CommandIds::Track::toggleMuteSelection) {
        toggleMuteSelection();
        return true;
    }
    return false;
}

bool TrackCommands::handlesCommand(juce::CommandID commandID) const {
    return commandID == CommandIds::Track::toggleSoloSelection
        || commandID == CommandIds::Track::clearAllSolo
        || commandID == CommandIds::Track::toggleMuteSelection;
}

void TrackCommands::toggleSoloSelection() {
    trackCommandManager.toggleSoloSelection(selectionManager.getSelectedTrackIds());
}

void TrackCommands::toggleMuteSelection() {
    trackCommandManager.toggleMuteSelection(selectionManager.getSelectedTrackIds());
}

void TrackCommands::clearAllSolo() {
    trackCommandManager.clearAllSolo();
}
