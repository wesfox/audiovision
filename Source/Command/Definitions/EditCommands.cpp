#include "EditCommands.h"

#include "Command/Commands.h"

namespace {
constexpr float kZoomStep = 0.1f;
constexpr float kScrollWheelStep = 1.0f;
}

EditCommands::EditCommands(Edit& edit, std::function<void()> toggleDebugWatchWindow)
    : edit(edit),
      toggleDebugWatchWindowCallback(std::move(toggleDebugWatchWindow)) {
}

void EditCommands::getAllCommands(juce::Array<juce::CommandID>& commands) {
    commands.add(CommandIds::EditView::zoomIn);
    commands.add(CommandIds::EditView::zoomOut);
    commands.add(CommandIds::EditView::scrollView);
    commands.add(CommandIds::EditView::toggleDebugWatchWindow);
}

void EditCommands::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) {
    if (commandID == CommandIds::EditView::zoomIn) {
        result.setInfo("Zoom In", "Zoom into the timeline", "Edit", 0);
        return;
    }
    if (commandID == CommandIds::EditView::zoomOut) {
        result.setInfo("Zoom Out", "Zoom out of the timeline", "Edit", 0);
        return;
    }
    if (commandID == CommandIds::EditView::scrollView) {
        result.setInfo("Scroll View", "Scroll the timeline view", "Edit", 0);
        return;
    }
    if (commandID == CommandIds::EditView::toggleDebugWatchWindow) {
        result.setInfo("Toggle Debug Watch", "Show or hide the debug watch window", "Edit", 0);
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
    if (info.commandID == CommandIds::EditView::scrollView) {
        return false;
    }
    if (info.commandID == CommandIds::EditView::toggleDebugWatchWindow) {
        toggleDebugWatchWindow();
        return true;
    }
    return false;
}

bool EditCommands::handlesCommand(juce::CommandID commandID) const {
    return commandID == CommandIds::EditView::zoomIn
        || commandID == CommandIds::EditView::zoomOut
        || commandID == CommandIds::EditView::scrollView
        || commandID == CommandIds::EditView::toggleDebugWatchWindow;
}

void EditCommands::zoom(float ratio) {
    edit.zoom(ratio);
}

bool EditCommands::handleWheelCommand(juce::CommandID commandID, float delta) {
    if (commandID == CommandIds::EditView::scrollView) {
        scrollView(delta);
        return true;
    }
    return false;
}

void EditCommands::scrollView(float delta) {
    const auto viewStart = edit.getViewStartSample();
    const auto viewEnd = edit.getViewEndSample();
    const auto viewLength = viewEnd - viewStart;
    if (viewLength <= 0) {
        // View length must be positive to scroll.
        jassert(false);
        return;
    }

    const auto offset = static_cast<int64>(std::llround(static_cast<double>(viewLength)
                                                        * kScrollWheelStep
                                                        * static_cast<double>(delta)));
    auto newStart = viewStart + offset;
    auto newEnd = viewEnd + offset;
    if (newStart < 0) {
        newStart = 0;
        newEnd = viewLength;
    }
    edit.getState().setViewRange(newStart, newEnd, nullptr);
}

void EditCommands::toggleDebugWatchWindow() {
    if (!toggleDebugWatchWindowCallback) {
        // Debug watch toggle callback must be configured before use.
        jassert(false);
        return;
    }
    toggleDebugWatchWindowCallback();
}
