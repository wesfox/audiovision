#include "SoloToggle.h"

SoloToggle::SoloToggle()
    : MultiStateToggleButton("S") {
    const auto textColour = juce::Colour(0xFF000000);
    setColors(static_cast<StateId>(State::Inactive), juce::Colour(0xFFEBE3FF), textColour);
    setColors(static_cast<StateId>(State::Solo), juce::Colour(0xFFFFDFAC), textColour);
    setColors(static_cast<StateId>(State::SoloSafe), juce::Colour(0x60EBE3FF), textColour);
    setStateCycle({
        static_cast<StateId>(State::Inactive),
        static_cast<StateId>(State::Solo)
    });
}

void SoloToggle::onSoloRequested(Callback callback) {
    onSoloRequestedCallback = std::move(callback);
}

void SoloToggle::onSoloSafeRequested(Callback callback) {
    onSoloSafeRequestedCallback = std::move(callback);
}

void SoloToggle::mouseDown(const juce::MouseEvent& e) {
    if (e.mods.isCommandDown()) {
        if (onSoloSafeRequestedCallback) {
            onSoloSafeRequestedCallback();
        } else {
            // Command-click should be handled by an external manager.
            jassertfalse;
        }
        return;
    }

    if (e.mods.isShiftDown()) {
        MultiStateToggleButton::mouseDown(e);
        return;
    }

    if (onSoloRequestedCallback) {
        onSoloRequestedCallback();
    } else {
        // Solo requests should be handled by an external manager.
        jassertfalse;
    }
}
