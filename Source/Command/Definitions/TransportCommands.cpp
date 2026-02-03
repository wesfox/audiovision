#include "TransportCommands.h"

#include "Command/Commands.h"
#include "Gui/Utils/CursorController.h"

#include <optional>

TransportCommands::TransportCommands(Edit& edit, CursorController& cursorController)
    : edit(edit),
      cursorController(cursorController) {
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
        // we should have a transport here
        jassert(false);
        return;
    }

    if (transport->isPlaying()) {
        transport->stop();
    } else {
        // get start sample
        int64_t startSample = cursorController.onPlayRequested();
        transport->setPlayheadSample(startSample);

        // play either with endSample (if we get a value)
        std::optional<int64_t> endSample;
         if (edit.getState().hasSelectionRange()) {
             endSample = edit.getState().getSelectionEndSample();

             if (!endSample.has_value()) {
                 // endSample should have a value in this case
                 jassert(false);
             }

             // is looping
             auto looping = edit.getState().getIsLooping();
             transport->play(endSample.value(), looping);
         } else {
             // or directly from the playhead if there is no selection
             transport->play();
        }

    }
}

void TransportCommands::toggleInsertionFollowsPlayback() {
    const bool nextValue = !edit.getState().getInsertionFollowsPlayback();
    edit.getState().setInsertionFollowsPlayback(nextValue, &edit.getUndoManager());
    juce::Logger::writeToLog("Insertion follows playback: " + juce::String(nextValue ? "On" : "Off"));
}
