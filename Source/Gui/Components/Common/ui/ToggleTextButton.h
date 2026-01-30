#pragma once

#include <JuceHeader.h>

class ToggleTextButton : public juce::Component {
public:
    ToggleTextButton(const String &textContent, juce::Colour untoggledBackgroundColor, juce::Colour untoggledTextColor);

    void resized() override;

    void paint(juce::Graphics& g) override;

    void mouseDown (const juce::MouseEvent& e) override;

    void setToggledColor(juce::Colour toggledBackgroundColor_, juce::Colour toggledTextColor_) {
        toggledBackgroundColor = toggledBackgroundColor_;
        toggledTextColor = toggledTextColor_;
    }

    void setUntoggledColor(const juce::Colour untoggledBackgroundColor_, const juce::Colour untoggledTextColor_) {
        untoggledBackgroundColor = untoggledBackgroundColor_;
        untoggledTextColor = untoggledTextColor_;
    }

    void setToggled(const bool newToggleStatus) {
        isToggled = newToggleStatus;
    }

private:
    String textContent;
    bool isToggled = false;

    juce::Colour untoggledBackgroundColor;
    juce::Colour untoggledTextColor;

    juce::Colour toggledBackgroundColor;
    juce::Colour toggledTextColor;
};