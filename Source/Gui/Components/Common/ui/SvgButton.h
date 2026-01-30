#pragma once

#include <JuceHeader.h>

#include "Gui/Assets/SvgFactory.h"

class SvgButton : public juce::Component {
public:
    /// Create a button that draws the SVG asset in the requested colour.
    /// @param asset SVG asset to render
    /// @param colour tint applied when loading the SVG
    SvgButton(SVG_Assets asset, juce::Colour colour);

    void paint(juce::Graphics& g) override;
    void mouseUp(const juce::MouseEvent& event) override;

    std::function<void()> onClick;

private:
    std::unique_ptr<juce::Drawable> drawable;
};
