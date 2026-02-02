#include "TransportCommands.h"

#include "Command/Commands.h"

#include <optional>

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
        const auto playheadSample = transport->getPlayheadSample();
    } else {
        int64_t startSample = edit.getState().getCursorSample();
        std::optional<int64_t> endSample;
        if (edit.getState().hasSelectionRange()) {
            const auto selectionStart = edit.getState().getSelectionStartSample();
            const auto selectionEnd = edit.getState().getSelectionEndSample();
            startSample = std::min(selectionStart, selectionEnd);
            const auto orderedEnd = std::max(selectionStart, selectionEnd);
            if (orderedEnd > startSample) {
                endSample = std::max<int64_t>(0, orderedEnd - 1);
            }
            playSelectionStartSample = startSample;
        } else {
            playSelectionStartSample.reset();
        }
        edit.getState().setCursorSample(startSample);
        transport->setPlayheadSample(startSample);
        transport->play(endSample);
    }
}

void TransportCommands::toggleInsertionFollowsPlayback() {
    const bool nextValue = !edit.getState().getInsertionFollowsPlayback();
    edit.getState().setInsertionFollowsPlayback(nextValue, &edit.getUndoManager());
    juce::Logger::writeToLog("Insertion follows playback: " + juce::String(nextValue ? "On" : "Off"));
}
