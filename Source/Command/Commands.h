#pragma once

#include <JuceHeader.h>

/// Command identifiers grouped by domain.
namespace CommandIds::Transport {
static constexpr juce::CommandID playPause = 0x2000;
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
}
