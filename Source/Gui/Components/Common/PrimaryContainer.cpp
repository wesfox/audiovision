#include "PrimaryContainer.h"

#include "Gui/Style/Font.h"

PrimaryContainer::PrimaryContainer(const Edit* edit) : edit(edit) {
}

void PrimaryContainer::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();
    const auto radius = 6.0f;
    const auto strokeColour = juce::Colour::fromString("#D0E6E3F0");

    g.setColour(juce::Colour::fromString("#FFF8F7FD"));
    g.fillRoundedRectangle(bounds, radius);

    g.setColour(strokeColour);
    g.drawRoundedRectangle(bounds, radius, 1.0f);

    auto headerBounds = bounds.removeFromTop(static_cast<float>(kHeaderHeight));
    g.setColour(juce::Colour::fromString("#FFF2F0FD"));
    g.fillRect(headerBounds);
    const auto accentBounds = headerBounds.withTrimmedLeft(5).removeFromLeft(2.0f).reduced(0.0f, 8.0f);
    g.setColour(juce::Colour::fromString("#CAB9B2E6"));
    g.fillRoundedRectangle(accentBounds, 1.0f);

    g.setColour(strokeColour);
    g.drawLine(bounds.getX(),
               headerBounds.getBottom(),
               bounds.getRight(),
               headerBounds.getBottom(),
               1.0f);

    if (title.isNotEmpty()) {
        const auto titleBounds = getLocalBounds().removeFromTop(kHeaderHeight).reduced(10.0f, 0.0f);
        g.setColour(juce::Colour::fromString("#FF2F2C3F"));
        g.setFont(Fonts::p(18.0f, Fonts::Weight::Bold));
        g.drawText(title, titleBounds.toNearestInt(), juce::Justification::centredLeft, false);
    }
}

void PrimaryContainer::setTitle(juce::String newTitle) {
    title = std::move(newTitle);
    repaint();
}

juce::Rectangle<int> PrimaryContainer::getContainerBounds() const {
    auto bounds = getLocalBounds();
    bounds.removeFromTop(kHeaderHeight);
    return bounds;
}
