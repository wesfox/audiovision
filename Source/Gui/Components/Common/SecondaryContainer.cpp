#include "SecondaryContainer.h"

#include "Gui/Style/Font.h"

SecondaryContainer::SecondaryContainer(const Edit* edit) : edit(edit) {
}

void SecondaryContainer::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat().reduced(0.5f);

    g.setColour(juce::Colours::white);
    g.fillRoundedRectangle(bounds, 6.0f);

    g.setColour(juce::Colour::fromString("#FFE1E4EE"));
    g.drawRoundedRectangle(bounds, 6.0f, 1.0f);

    if (title.isNotEmpty()) {
        auto titleBounds = getLocalBounds().withTrimmedTop(4).removeFromTop(16).withTrimmedLeft(4);
        g.setColour(juce::Colour::fromString("#FF2F2C3F"));
        g.setFont(Fonts::p(16.0f, Fonts::Weight::Regular));
        g.drawText(title, titleBounds, juce::Justification::centredLeft, false);
    }
}

void SecondaryContainer::setTitle(juce::String newTitle) {
    title = std::move(newTitle);
    repaint();
}

juce::Rectangle<int> SecondaryContainer::getContainerBounds() const {
    auto bounds = getLocalBounds().reduced(0, 4);
    if (title.isNotEmpty()) {
        bounds.removeFromTop(20);
    }
    return bounds;
}
