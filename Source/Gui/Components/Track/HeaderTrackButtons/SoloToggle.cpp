#include "SoloToggle.h"

SoloToggle::SoloToggle()
    : MultiStateToggleButton("S") {
    const auto textColour = juce::Colour(0xFF000000);
    setColors(static_cast<StateId>(State::Inactive), juce::Colour(0xFFEBE3FF), textColour);
    setColors(static_cast<StateId>(State::Solo), juce::Colour(0xFFFFDFAC), textColour);
    setColors(static_cast<StateId>(State::SoloSafe), juce::Colour(0x60EBE3FF), textColour);
    setStateCycle({
        static_cast<StateId>(State::Inactive),
        static_cast<StateId>(State::Solo),
        static_cast<StateId>(State::SoloSafe)
    });
}
