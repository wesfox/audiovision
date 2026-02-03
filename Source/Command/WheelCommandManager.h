#pragma once

#include <JuceHeader.h>

/// Routes mouse wheel gestures to command IDs.
class WheelCommandManager {
public:
    /// Handles wheel commands that need delta input.
    class Target {
    public:
        virtual ~Target() = default;
        virtual bool handleWheelCommand(juce::CommandID commandId, float delta) = 0;
    };

    /// Create a wheel command manager.
    /// @param commandManager command manager used to invoke commands
    /// @param target optional target for wheel commands with delta
    explicit WheelCommandManager(juce::ApplicationCommandManager& commandManager, Target* target = nullptr);

    /// Handle a wheel gesture and dispatch matching commands.
    /// @param event mouse event containing modifier state
    /// @param details wheel details used for direction
    bool handleWheel(const juce::MouseEvent& event, const juce::MouseWheelDetails& details);

private:
    void invoke(juce::CommandID commandId, float delta);

    juce::ApplicationCommandManager& commandManager;
    Target* target = nullptr;
};
