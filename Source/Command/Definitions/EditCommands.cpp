#include "EditCommands.h"

#include "Command/Commands.h"

namespace {
constexpr float kZoomStep = 0.1f;
}

EditCommands::EditCommands(Edit& edit)
    : edit(edit) {
}

void EditCommands::getAllCommands(juce::Array<juce::CommandID>& commands) {
    commands.add(CommandIds::EditView::zoomIn);
    commands.add(CommandIds::EditView::zoomOut);
}

void EditCommands::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) {
    if (commandID == CommandIds::EditView::zoomIn) {
        result.setInfo("Zoom In", "Zoom into the timeline", "Edit", 0);
        return;
    }
    if (commandID == CommandIds::EditView::zoomOut) {
        result.setInfo("Zoom Out", "Zoom out of the timeline", "Edit", 0);
    }
}

bool EditCommands::perform(const juce::ApplicationCommandTarget::InvocationInfo& info) {
    if (info.commandID == CommandIds::EditView::zoomIn) {
        zoom(-kZoomStep);
        return true;
    }
    if (info.commandID == CommandIds::EditView::zoomOut) {
        zoom(kZoomStep);
        return true;
    }
    return false;
}

bool EditCommands::handlesCommand(juce::CommandID commandID) const {
    return commandID == CommandIds::EditView::zoomIn
        || commandID == CommandIds::EditView::zoomOut;
}

void EditCommands::zoom(float ratio) {
    edit.zoom(ratio);
}
