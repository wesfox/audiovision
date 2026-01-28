#include "SvgIcon.h"

SvgIcon::SvgIcon(SVG_Assets asset, juce::Colour colour)
    : drawable(SvgFactory::load(asset, colour)) {
}

void SvgIcon::paint(juce::Graphics& g) {
    if (drawable == nullptr) {
        return;
    }
    drawable->drawWithin(g,
                         getLocalBounds().toFloat(),
                         juce::RectanglePlacement::centred,
                         1.0f);
}
