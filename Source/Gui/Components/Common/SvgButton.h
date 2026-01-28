#pragma once

#include <JuceHeader.h>

#include "Gui/Assets/SvgFactory.h"

class SvgButton : public juce::Component {
public:
    SvgButton(SVG_Assets asset, juce::Colour colour);

    void paint(juce::Graphics& g) override;

private:
    std::unique_ptr<juce::Drawable> drawable;
};
