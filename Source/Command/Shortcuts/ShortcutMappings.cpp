#include "ShortcutMappings.h"

#include "Command/Commands.h"

void ShortcutMappings::applyDefaultMappings(juce::ApplicationCommandManager& manager) {
    if (auto* mappings = manager.getKeyMappings()) {
        mappings->addKeyPress(CommandIds::Transport::playPause, juce::KeyPress(juce::KeyPress::spaceKey));
        mappings->addKeyPress(CommandIds::Transport::playPauseLooping,
                              juce::KeyPress('l', juce::ModifierKeys::ctrlModifier, 0));
        mappings->addKeyPress(CommandIds::Transport::toggleInsertionFollowsPlayback,
                              juce::KeyPress('n', juce::ModifierKeys(), 0));
        mappings->addKeyPress(CommandIds::EditView::zoomIn, juce::KeyPress('t', juce::ModifierKeys(), 0));
        mappings->addKeyPress(CommandIds::EditView::zoomOut, juce::KeyPress('r', juce::ModifierKeys(), 0));
        mappings->addKeyPress(CommandIds::Track::toggleSoloSelection,
                              juce::KeyPress('S', juce::ModifierKeys::shiftModifier, 0));
        mappings->addKeyPress(CommandIds::Track::clearAllSolo,
                              juce::KeyPress('S',
                                             juce::ModifierKeys::altModifier | juce::ModifierKeys::shiftModifier,
                                             0));
        mappings->addKeyPress(CommandIds::Track::toggleMuteSelection,
                              juce::KeyPress('M', juce::ModifierKeys::shiftModifier, 0));
        mappings->addKeyPress(CommandIds::EditView::toggleDebugWatchWindow,
                              juce::KeyPress('D',
                                             juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier,
                                             0));
        mappings->addKeyPress(CommandIds::EditView::splitClipsAtCursorOrSelection,
                              juce::KeyPress('b', juce::ModifierKeys(), 0));
        mappings->addKeyPress(CommandIds::EditView::healClip,
                              juce::KeyPress('h', juce::ModifierKeys::commandModifier, 0));
        mappings->addKeyPress(CommandIds::EditView::deleteClipsInSelection,
                              juce::KeyPress(juce::KeyPress::backspaceKey, juce::ModifierKeys(), 0));
        mappings->addKeyPress(CommandIds::EditView::trimToSelection,
                              juce::KeyPress('t', juce::ModifierKeys::commandModifier, 0));
        mappings->addKeyPress(CommandIds::EditView::trimHeadToCursor,
                              juce::KeyPress('a', juce::ModifierKeys(), 0));
        mappings->addKeyPress(CommandIds::EditView::trimTailToCursor,
                              juce::KeyPress('s', juce::ModifierKeys(), 0));
        mappings->addKeyPress(CommandIds::EditView::selectPreviousTrack,
                              juce::KeyPress('p', juce::ModifierKeys(), 0));
        mappings->addKeyPress(CommandIds::EditView::selectPreviousTrack,
                              juce::KeyPress('p', juce::ModifierKeys::shiftModifier, 0));
        mappings->addKeyPress(CommandIds::EditView::selectNextTrack,
                              juce::KeyPress(';', juce::ModifierKeys(), 0));
        mappings->addKeyPress(CommandIds::EditView::selectNextTrack,
                              juce::KeyPress(':',
                                             juce::ModifierKeys::shiftModifier,
                                             0));
        mappings->addKeyPress(CommandIds::EditView::moveNextCutToCursor,
                              juce::KeyPress('s', juce::ModifierKeys::ctrlModifier, 0));
        mappings->addKeyPress(CommandIds::EditView::movePreviousCutToCursor,
                              juce::KeyPress('a', juce::ModifierKeys::ctrlModifier, 0));
        mappings->addKeyPress(CommandIds::EditView::tabToNextCut,
                              juce::KeyPress(juce::KeyPress::tabKey, juce::ModifierKeys(), 0));
        mappings->addKeyPress(CommandIds::EditView::tabToNextCut,
                              juce::KeyPress(juce::KeyPress::tabKey, juce::ModifierKeys::shiftModifier, 0));
        mappings->addKeyPress(CommandIds::EditView::tabToPreviousCut,
                              juce::KeyPress(juce::KeyPress::tabKey, juce::ModifierKeys::altModifier, 0));
        mappings->addKeyPress(CommandIds::EditView::tabToPreviousCut,
                              juce::KeyPress(juce::KeyPress::tabKey,
                                             juce::ModifierKeys::altModifier | juce::ModifierKeys::shiftModifier,
                                             0));
        mappings->addKeyPress(CommandIds::EditView::moveCursorToSessionStart,
                              juce::KeyPress(juce::KeyPress::returnKey, juce::ModifierKeys(), 0));
        mappings->addKeyPress(CommandIds::EditView::moveCursorToSessionStart,
                              juce::KeyPress(juce::KeyPress::returnKey,
                                             juce::ModifierKeys::shiftModifier,
                                             0));
        mappings->addKeyPress(CommandIds::EditView::moveCursorToSessionEnd,
                              juce::KeyPress(juce::KeyPress::returnKey,
                                             juce::ModifierKeys::altModifier,
                                             0));
        mappings->addKeyPress(CommandIds::EditView::moveCursorToSessionEnd,
                              juce::KeyPress(juce::KeyPress::returnKey,
                                             juce::ModifierKeys::altModifier | juce::ModifierKeys::shiftModifier,
                                             0));
        mappings->addKeyPress(CommandIds::EditView::undo,
                              juce::KeyPress('z', juce::ModifierKeys(), 0));
        mappings->addKeyPress(CommandIds::EditView::undo,
                              juce::KeyPress('z', juce::ModifierKeys::commandModifier, 0));
        mappings->addKeyPress(CommandIds::Project::save,
                              juce::KeyPress('s', juce::ModifierKeys::commandModifier, 0));
    }
}

const std::vector<ShortcutMappings::WheelShortcut>& ShortcutMappings::getDefaultWheelShortcuts() {
    static const std::vector<WheelShortcut> shortcuts = {
        {
            juce::ModifierKeys(juce::ModifierKeys::shiftModifier),
            WheelShortcut::WheelDirection::WheelUp,
            CommandIds::EditView::scrollView
        },
        {
            juce::ModifierKeys(juce::ModifierKeys::shiftModifier),
            WheelShortcut::WheelDirection::WheelDown,
            CommandIds::EditView::scrollView
        },
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
