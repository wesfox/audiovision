#pragma once

#include <JuceHeader.h>

#include "Gui/Assets/SvgFactory.h"

/// Displays an SVG asset with a specific colour.
class SvgIcon : public juce::Component {
public:
    SvgIcon(SVG_Assets asset, juce::Colour colour);

    void paint(juce::Graphics& g) override;

private:
    std::unique_ptr<juce::Drawable> drawable;
};
