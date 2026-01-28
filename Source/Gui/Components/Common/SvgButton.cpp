#include "SvgButton.h"

SvgButton::SvgButton(SVG_Assets asset, juce::Colour colour)
    : drawable(SvgFactory::load(asset, colour)) {
}

void SvgButton::paint(juce::Graphics& g) {
    if (drawable == nullptr) {
        return;
    }
    drawable->drawWithin(g,
                         getLocalBounds().toFloat(),
                         juce::RectanglePlacement::centred,
                         1.0f);
}
