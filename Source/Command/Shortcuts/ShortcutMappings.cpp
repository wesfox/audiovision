#include "ShortcutMappings.h"

#include "Command/Commands.h"

void ShortcutMappings::applyDefaultMappings(juce::ApplicationCommandManager& manager) {
    if (auto* mappings = manager.getKeyMappings()) {
        mappings->addKeyPress(CommandIds::Transport::playPause, juce::KeyPress(juce::KeyPress::spaceKey));
        mappings->addKeyPress(CommandIds::EditView::zoomIn, juce::KeyPress('t', juce::ModifierKeys(), 0));
        mappings->addKeyPress(CommandIds::EditView::zoomOut, juce::KeyPress('r', juce::ModifierKeys(), 0));
    }
}

const std::vector<ShortcutMappings::WheelShortcut>& ShortcutMappings::getDefaultWheelShortcuts() {
    static const std::vector<WheelShortcut> shortcuts = {
        {
            juce::ModifierKeys(juce::ModifierKeys::altModifier | juce::ModifierKeys::shiftModifier),
            WheelShortcut::WheelDirection::WheelUp,
            CommandIds::Waveform::increaseScale
        },
        {
            juce::ModifierKeys(juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier),
            WheelShortcut::WheelDirection::WheelUp,
            CommandIds::Waveform::increaseScale
        },
        {
            juce::ModifierKeys(juce::ModifierKeys::altModifier | juce::ModifierKeys::shiftModifier),
            WheelShortcut::WheelDirection::WheelDown,
            CommandIds::Waveform::decreaseScale
        },
        {
            juce::ModifierKeys(juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier),
            WheelShortcut::WheelDirection::WheelDown,
            CommandIds::Waveform::decreaseScale
        }
    };
    return shortcuts;
}
