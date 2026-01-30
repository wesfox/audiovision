#include "ToggleTextButton.h"

// ------------------------ MainComponent Implementation ------------------------

ToggleTextButton::ToggleTextButton(const String &textContent, const juce::Colour untoggledBackgroundColor, const juce::Colour untoggledTextColor) :
    textContent(textContent),
    untoggledBackgroundColor(untoggledBackgroundColor),
    untoggledTextColor(untoggledTextColor) {
}

void ToggleTextButton::resized() {
}

void ToggleTextButton::paint(juce::Graphics &g) {
    auto b = getLocalBounds();
    g.setFont(14.0f);
    if (isToggled) {
        g.setColour(toggledBackgroundColor);
        g.fillRoundedRectangle(b.toFloat(), 10.0);
        g.setColour(toggledTextColor);
        g.drawText(textContent, b, Justification::centred);
    } else {
        g.setColour(untoggledBackgroundColor);
        g.fillRoundedRectangle(b.toFloat(), 10.0);
        g.setColour(untoggledTextColor);
        g.drawText(textContent, b, Justification::centred);
    }
}

void ToggleTextButton::mouseDown(const juce::MouseEvent &e) {
    isToggled = !isToggled;
    repaint();
}
