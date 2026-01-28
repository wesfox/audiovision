#include "TimecodesDisplay.h"

#include "Gui/Style/Font.h"

TimecodesDisplay::TimecodesDisplay() = default;

void TimecodesDisplay::paint(juce::Graphics& g) {
    const auto bounds = getLocalBounds().toFloat();
    const auto lineColour = juce::Colour::fromString("#FFE7E1FB");

    g.setColour(juce::Colour::fromString("#FF2F2C3F"));
    g.fillRoundedRectangle(bounds, kCornerRadius);

    g.setColour(lineColour);
    const float dividerX = bounds.getX() + static_cast<float>(kLeftWidth);
    g.drawLine(dividerX, bounds.getY(), dividerX, bounds.getBottom(), 1.0f);

    auto leftBounds = getLocalBounds().removeFromLeft(kLeftWidth).reduced(12, 8);
    g.setColour(juce::Colour::fromString("#FFF6F4FF"));
    g.setFont(Fonts::p(12.0f, Fonts::Weight::Regular));
    g.drawText("Timecode", leftBounds.removeFromTop(12), juce::Justification::centredLeft, false);

    g.setFont(Fonts::p(24.0f, Fonts::Weight::Bold));
    g.drawText(timecode, leftBounds.withTrimmedTop(4), juce::Justification::centredLeft, false);

    auto rightBounds = getLocalBounds().withTrimmedLeft(kLeftWidth).withWidth(kRightWidth).reduced(12, 6);
    const auto rowHeight = rightBounds.getHeight() / 3;
    const auto labelColour = juce::Colour::fromString("#FFE7E1FB");
    const auto valueColour = juce::Colour::fromString("#FFF6F4FF");

    auto drawRow = [&](juce::Rectangle<int> rowBounds, const juce::String& label,
                       const juce::String& value) {
        auto labelBounds = rowBounds.removeFromLeft(kLabelWidth);
        g.setFont(Fonts::p(11.0f, Fonts::Weight::Regular));
        g.setColour(labelColour);
        g.drawText(label, labelBounds, juce::Justification::centredRight, false);

        g.setFont(Fonts::p(11.0f, Fonts::Weight::Bold));
        g.setColour(valueColour);
        g.drawText(value, rowBounds, juce::Justification::centredRight, false);
    };

    drawRow(rightBounds.removeFromTop(rowHeight), "In:", inTimecode);
    drawRow(rightBounds.removeFromTop(rowHeight), "Out:", outTimecode);
    drawRow(rightBounds.removeFromTop(rowHeight), "Length:", lengthTimecode);
}

void TimecodesDisplay::setTimecode(juce::String newTimecode) {
    timecode = std::move(newTimecode);
    repaint();
}

void TimecodesDisplay::setInTimecode(juce::String newTimecode) {
    inTimecode = std::move(newTimecode);
    repaint();
}

void TimecodesDisplay::setOutTimecode(juce::String newTimecode) {
    outTimecode = std::move(newTimecode);
    repaint();
}

void TimecodesDisplay::setLengthTimecode(juce::String newTimecode) {
    lengthTimecode = std::move(newTimecode);
    repaint();
}
