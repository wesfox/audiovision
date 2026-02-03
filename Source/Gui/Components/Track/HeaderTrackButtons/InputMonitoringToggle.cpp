#include "InputMonitoringToggle.h"

InputMonitoringToggle::InputMonitoringToggle()
    : MultiStateToggleButton("I") {
    const auto textColour = juce::Colour(0xFF000000);
    setColors(static_cast<StateId>(State::Inactive), juce::Colour(0xFFE7E1FB), textColour);
    setColors(static_cast<StateId>(State::Active), juce::Colour(0xFF68C39A), textColour);
}
