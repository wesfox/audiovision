#pragma once

#include <JuceHeader.h>

/// Displays a rectangular toggle-style label button.
class RectToggleButton : public juce::Component {
public:
    explicit RectToggleButton(juce::String text,
                              juce::Colour activeBackground = juce::Colour::fromString("#B0C8D9B8"));

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;

    void setActive(bool shouldBeActive);
    bool isActive() const;

private:
    void drawInsetShadow(juce::Graphics& g,
                         juce::Rectangle<float> bounds,
                         juce::Rectangle<float> innerBounds,
                         float radius) const;

    juce::String label;
    bool active = false;
    juce::Colour activeBackground;
};
