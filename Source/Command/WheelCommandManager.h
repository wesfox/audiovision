#pragma once

#include <JuceHeader.h>

/// Routes mouse wheel gestures to command IDs.
class WheelCommandManager {
public:
    /// Create a wheel command manager.
    /// @param commandManager command manager used to invoke commands
    explicit WheelCommandManager(juce::ApplicationCommandManager& commandManager);

    /// Handle a wheel gesture and dispatch matching commands.
    /// @param event mouse event containing modifier state
    /// @param details wheel details used for direction
    bool handleWheel(const juce::MouseEvent& event, const juce::MouseWheelDetails& details);

private:
    void invoke(juce::CommandID commandId);

    juce::ApplicationCommandManager& commandManager;
};
