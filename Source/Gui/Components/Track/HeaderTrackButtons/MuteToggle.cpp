#include "MuteToggle.h"

MuteToggle::MuteToggle()
    : MultiStateToggleButton("ON") {
    const auto textColour = juce::Colour(0xFF000000);
    setColors(static_cast<StateId>(State::Active), juce::Colour(0xFF7DE38F), textColour);
    setColors(static_cast<StateId>(State::SoloMute), juce::Colour(0xFFD9E9DB), textColour);
    setColors(static_cast<StateId>(State::Mute), juce::Colour(0x60EBE3FF), textColour);
    setStateCycle({
        static_cast<StateId>(State::Active),
        static_cast<StateId>(State::SoloMute),
        static_cast<StateId>(State::Mute)
    });
}
