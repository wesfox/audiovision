#include "ArmedToggle.h"

ArmedToggle::ArmedToggle()
    : MultiStateToggleButton("R") {
    const auto textColour = juce::Colour(0xFF000000);
    setColors(static_cast<StateId>(State::Inactive), juce::Colour(0xFFE7E1FB), textColour);
    setColors(static_cast<StateId>(State::Active), juce::Colour(0xFFFF8787), textColour);
}
