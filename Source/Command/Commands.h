#pragma once

#include <JuceHeader.h>

/// Command identifiers grouped by domain.
namespace CommandIds::Transport {
static constexpr juce::CommandID playPause = 0x2000;
static constexpr juce::CommandID toggleInsertionFollowsPlayback = 0x2001;
static constexpr juce::CommandID playPauseLooping = 0x2002;
}

/// Command identifiers for waveform editing.
namespace CommandIds::Waveform {
static constexpr juce::CommandID increaseScale = 0x2100;
static constexpr juce::CommandID decreaseScale = 0x2101;
}

/// Command identifiers for edit view actions.
namespace CommandIds::EditView {
static constexpr juce::CommandID zoomIn = 0x2200;
static constexpr juce::CommandID zoomOut = 0x2201;
static constexpr juce::CommandID scrollView = 0x2202;
static constexpr juce::CommandID toggleDebugWatchWindow = 0x2203;
}

/// Command identifiers for track actions.
namespace CommandIds::Track {
static constexpr juce::CommandID toggleSoloSelection = 0x2300;
static constexpr juce::CommandID clearAllSolo = 0x2301;
static constexpr juce::CommandID toggleMuteSelection = 0x2302;
}
