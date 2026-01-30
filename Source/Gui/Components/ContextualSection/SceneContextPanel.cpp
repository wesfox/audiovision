#include "SceneContextPanel.h"

#include "Gui/Style/Font.h"

SceneContextPanel::SceneContextPanel() {
    setTitle("Scene 14");

    buttons.emplace_back(std::make_unique<RoundedButton>("Nicolas"));
    buttons.emplace_back(std::make_unique<RoundedButton>(juce::String::fromUTF8("Clémence")));
    buttons.emplace_back(std::make_unique<RoundedButton>("+"));

    for (const auto& button : buttons) {
        addAndMakeVisible(*button);
    }
}

void SceneContextPanel::paint(juce::Graphics& g) {
    PrimaryContainer::paint(g);

    auto contentBounds = getContainerBounds().reduced(8, 8);
    auto textBounds = contentBounds.withHeight(contentBounds.getHeight() - kButtonRowHeight);

    g.setColour(juce::Colour::fromString("#FF2F2C3F"));
    g.setFont(Fonts::p(12.0f, Fonts::Weight::Regular));
    g.drawText(description, textBounds.removeFromTop(18), juce::Justification::centredLeft, false);

    g.setFont(Fonts::p(12.0f, Fonts::Weight::Regular));
    g.drawText(charactersLabel, textBounds.withTrimmedBottom(4), juce::Justification::bottomLeft, false);
}

void SceneContextPanel::resized() {
    auto contentBounds = getContainerBounds();
    const int buttonRowHeight = 25;
    auto buttonsBounds = contentBounds.removeFromBottom(buttonRowHeight);

    juce::Grid grid;
    grid.templateRows = { 1_fr };
    for (size_t index = 0; index < buttons.size(); ++index) {
        grid.templateColumns.add(1_fr);
    }
    grid.setGap(juce::Grid::Px(6));

    for (const auto& button : buttons) {
        grid.items.add(juce::GridItem(*button));
    }

    grid.performLayout(buttonsBounds);
}
