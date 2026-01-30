#include "RectToggleButton.h"

#include "Gui/Style/Font.h"

RectToggleButton::RectToggleButton(juce::String text, juce::Colour activeBackground)
    : label(std::move(text)),
      activeBackground(activeBackground) {
}

void RectToggleButton::paint(juce::Graphics& g) {
    const auto bounds = getLocalBounds().toFloat().reduced(0.5f);
    const auto radius = 2.0f;
    const auto activeFill = activeBackground;
    const auto activeStroke = activeFill.darker(0.15f);
    const auto inactiveFill = juce::Colour::fromString("#FFEDEBF7");
    const auto inactiveStroke = inactiveFill.darker(0.15f);
    const auto innerBounds = bounds.reduced(1.5f);

    g.setColour(active ? activeFill : inactiveFill);
    g.fillRoundedRectangle(bounds, radius);

    g.setColour(active ? activeStroke : inactiveStroke);
    g.drawRoundedRectangle(bounds, radius, 1.0f);

    drawInsetShadow(g, bounds, innerBounds, radius);

    auto textColour = juce::Colour::fromString("#FF2F2C3F");
    if (!active) {
        textColour = textColour.withAlpha(0.5f);
    }
    g.setColour(textColour);
    g.setFont(Fonts::p(12.0f, Fonts::Weight::Regular));
    g.drawText(label, getLocalBounds(), juce::Justification::centred, false);
}

void RectToggleButton::mouseDown(const juce::MouseEvent& event) {
    juce::Component::mouseDown(event);
    setActive(!active);
}

void RectToggleButton::setActive(bool shouldBeActive) {
    if (active == shouldBeActive) {
        return;
    }
    active = shouldBeActive;
    repaint();
}

bool RectToggleButton::isActive() const {
    return active;
}

void RectToggleButton::drawInsetShadow(juce::Graphics& g,
                                       juce::Rectangle<float> bounds,
                                       juce::Rectangle<float> innerBounds,
                                       float radius) const {
    const float thickness = 6.0f;
    const auto shadowColour = juce::Colour::fromString("#4A4A4A").withAlpha(0.1f);
    juce::Path innerPath;
    innerPath.addRoundedRectangle(innerBounds, std::max(0.0f, radius - 0.75f));

    auto drawBand = [&](juce::Rectangle<float> band, juce::Point<float> start, juce::Point<float> end) {
        g.saveState();
        g.reduceClipRegion(innerPath);
        g.reduceClipRegion(band.toNearestInt());
        juce::ColourGradient gradient(shadowColour, start, juce::Colours::transparentBlack, end, false);
        g.setGradientFill(gradient);
        g.fillRect(band);
        g.restoreState();
    };

    drawBand(bounds.withHeight(thickness),
             { bounds.getX(), bounds.getY() },
             { bounds.getX(), bounds.getY() + thickness });

    drawBand(bounds.withY(bounds.getBottom() - thickness).withHeight(thickness),
             { bounds.getX(), bounds.getBottom() },
             { bounds.getX(), bounds.getBottom() - thickness });

    drawBand(bounds.withWidth(thickness),
             { bounds.getX(), bounds.getY() },
             { bounds.getX() + thickness, bounds.getY() });

    drawBand(bounds.withX(bounds.getRight() - thickness).withWidth(thickness),
             { bounds.getRight(), bounds.getY() },
             { bounds.getRight() - thickness, bounds.getY() });
}
