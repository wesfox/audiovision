#include "TransportCommands.h"

#include "Command/Commands.h"

TransportCommands::TransportCommands(Edit& edit)
    : edit(edit) {
}

void TransportCommands::getAllCommands(juce::Array<juce::CommandID>& commands) {
    commands.add(CommandIds::Transport::playPause);
}

void TransportCommands::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) {
    if (commandID == CommandIds::Transport::playPause) {
        result.setInfo("Play/Pause", "Toggle transport playback", "Transport", 0);
        result.addDefaultKeypress(juce::KeyPress::spaceKey, 0);
    }
}

bool TransportCommands::perform(const juce::ApplicationCommandTarget::InvocationInfo& info) {
    if (info.commandID == CommandIds::Transport::playPause) {
        togglePlayPause();
        return true;
    }
    return false;
}

bool TransportCommands::handlesCommand(juce::CommandID commandID) const {
    return commandID == CommandIds::Transport::playPause;
}

void TransportCommands::togglePlayPause() {
    const auto transport = edit.getTransport();
    if (!transport) {
        return;
    }
    if (transport->isPlaying()) {
        transport->stop();
    } else {
        transport->play();
    }
}
