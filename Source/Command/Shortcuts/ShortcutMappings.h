#pragma once

#include <JuceHeader.h>

/// Centralizes default key mappings for commands.
namespace ShortcutMappings {
/// Register default key mappings with a command manager.
/// @param manager command manager to configure
void applyDefaultMappings(juce::ApplicationCommandManager& manager);

/// Defines a wheel shortcut mapping.
struct WheelShortcut {
    juce::ModifierKeys modifiers;
    enum class WheelDirection {
        WheelUp,
        WheelDown
    };
    WheelDirection direction;
    juce::CommandID command;
};

/// Default wheel shortcuts for command dispatch.
const std::vector<WheelShortcut>& getDefaultWheelShortcuts();
}
