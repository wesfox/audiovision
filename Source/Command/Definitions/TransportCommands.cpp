#include "TransportCommands.h"

#include "Command/Commands.h"

TransportCommands::TransportCommands(Edit& edit)
    : edit(edit) {
}

void TransportCommands::getAllCommands(juce::Array<juce::CommandID>& commands) {
    commands.add(CommandIds::Transport::playPause);
    commands.add(CommandIds::Transport::toggleInsertionFollowsPlayback);
}

void TransportCommands::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) {
    if (commandID == CommandIds::Transport::playPause) {
        result.setInfo("Play/Pause", "Toggle transport playback", "Transport", 0);
        result.addDefaultKeypress(juce::KeyPress::spaceKey, 0);
        return;
    }
    if (commandID == CommandIds::Transport::toggleInsertionFollowsPlayback) {
        result.setInfo("Toggle insertion follows playback",
                       "Toggle whether the playhead follows playback on stop",
                       "Transport",
                       0);
    }
}

bool TransportCommands::perform(const juce::ApplicationCommandTarget::InvocationInfo& info) {
    if (info.commandID == CommandIds::Transport::playPause) {
        togglePlayPause();
        return true;
    }
    if (info.commandID == CommandIds::Transport::toggleInsertionFollowsPlayback) {
        toggleInsertionFollowsPlayback();
        return true;
    }
    return false;
}

bool TransportCommands::handlesCommand(juce::CommandID commandID) const {
    return commandID == CommandIds::Transport::playPause
        || commandID == CommandIds::Transport::toggleInsertionFollowsPlayback;
}

void TransportCommands::togglePlayPause() {
    const auto transport = edit.getTransport();
    if (!transport) {
        return;
    }
    const auto followsPlayback = edit.getState().getInsertionFollowsPlayback();
    if (transport->isPlaying()) {
        const auto currentSample = transport->getCursorPosition();
        transport->stop();
        if (followsPlayback) {
            transport->setCursorPosition(currentSample);
        } else if (playStartSample.has_value()) {
            transport->setCursorPosition(*playStartSample);
        }
    } else {
        playStartSample = transport->getCursorPosition();
        transport->play();
    }
}

void TransportCommands::toggleInsertionFollowsPlayback() {
    const bool nextValue = !edit.getState().getInsertionFollowsPlayback();
    edit.getState().setInsertionFollowsPlayback(nextValue, &edit.getUndoManager());
    juce::Logger::writeToLog("Insertion follows playback: " + juce::String(nextValue ? "On" : "Off"));
}
