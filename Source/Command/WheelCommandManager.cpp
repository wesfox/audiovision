#include "WheelCommandManager.h"

#include "Command/Shortcuts/ShortcutMappings.h"

WheelCommandManager::WheelCommandManager(juce::ApplicationCommandManager& commandManager)
    : commandManager(commandManager) {
}

bool WheelCommandManager::handleWheel(const juce::MouseEvent& event, const juce::MouseWheelDetails& details) {
    const float delta = (std::abs(details.deltaY) >= std::abs(details.deltaX))
        ? details.deltaY
        : details.deltaX;

    ShortcutMappings::WheelShortcut::WheelDirection direction;
    if (delta > 0.0f) {
        direction = ShortcutMappings::WheelShortcut::WheelDirection::WheelUp;
    } else if (delta < 0.0f) {
        direction = ShortcutMappings::WheelShortcut::WheelDirection::WheelDown;
    } else {
        return false;
    }

    const ShortcutMappings::WheelShortcut* match = nullptr;
    for (const auto& shortcut : ShortcutMappings::getDefaultWheelShortcuts()) {
        if (event.mods.withoutMouseButtons() != shortcut.modifiers) {
            continue;
        }
        if (shortcut.direction != direction) {
            continue;
        }
        if (match != nullptr) {
            jassert(false);
            break;
        }
        match = &shortcut;
    }

    if (match == nullptr) {
        return false;
    }

    invoke(match->command);
    return true;
}

void WheelCommandManager::invoke(juce::CommandID commandId) {
    commandManager.invokeDirectly(commandId, true);
}
