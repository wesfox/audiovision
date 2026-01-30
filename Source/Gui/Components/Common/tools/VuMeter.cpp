#include "VuMeter.h"

#include <random>

namespace {
float randomVolume() {
    static std::random_device device;
    static std::mt19937 engine(device());
    static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    return distribution(engine);
}
}

VuMeter::VuMeter() : volume(randomVolume()) {
}

void VuMeter::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat().reduced(0.5f);

    g.setColour(juce::Colour::fromString("#FFECEAF6"));
    g.fillRoundedRectangle(bounds, 6.0f);

    const auto clippedVolume = std::clamp(volume, 0.0f, 1.0f);
    const float fillHeight = bounds.getHeight() * clippedVolume;
    auto fillBounds = bounds.withY(bounds.getBottom() - fillHeight).withHeight(fillHeight);

    juce::ColourGradient gradient(
        juce::Colour::fromString("#FFB6D579"),
        bounds.getBottomLeft(),
        juce::Colour::fromString("#FFE67A7A"),
        bounds.getTopLeft(),
        false);
    gradient.addColour(0.4, juce::Colour::fromString("#FFB6D579"));
    gradient.addColour(0.65, juce::Colour::fromString("#FFE6D96A"));
    gradient.addColour(1.0, juce::Colour::fromString("#FFE67A7A"));

    g.saveState();
    g.reduceClipRegion(fillBounds.toNearestInt());
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, 6.0f);
    g.restoreState();


    g.setColour(juce::Colour::fromString("#FFFFFFFF"));
    g.drawLine(0,bounds.getHeight()*0.1,bounds.getWidth(), bounds.getHeight()*0.1, 2.0f);

    g.setColour(juce::Colour::fromString("#60CFCBE3"));
    g.drawRoundedRectangle(bounds, 6.0f, 1.0f);
}

void VuMeter::setVolume(float newVolume) {
    volume = newVolume;
    repaint();
}
