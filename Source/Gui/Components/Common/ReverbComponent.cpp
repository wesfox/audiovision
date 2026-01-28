#include "ReverbComponent.h"

ReverbComponent::ReverbComponent(const Edit* edit)
    : SecondaryContainer(edit) {
    setTitle("Reverb A");

    firstButton = std::make_unique<RoundedButton>("+");
    addButton = std::make_unique<RoundedButton>("+");

    addAndMakeVisible(*firstButton);
    addAndMakeVisible(*addButton);
    addAndMakeVisible(fader);

    refreshButtons();
}

void ReverbComponent::resized() {
    auto bounds = getLocalBounds();
    auto faderBounds = bounds.removeFromRight(bounds.getWidth() - 89);
    fader.setBounds(faderBounds);

    auto contentBounds = getContainerBounds();
    auto listBounds = contentBounds.removeFromLeft(89).reduced(8, 10);
    const int rowHeight = 32;
    const int rowGap = 8;
    const int labelWidth = 18;

    auto firstRow = listBounds.removeFromTop(rowHeight);
    listBounds.removeFromTop(rowGap);
    auto secondRow = listBounds.removeFromTop(rowHeight);

    auto layoutRow = [&](juce::Rectangle<int> rowBounds, RoundedButton* button) {
        rowBounds.removeFromLeft(labelWidth);
        auto buttonBounds = rowBounds.withTrimmedLeft(2);
        button->setBounds(buttonBounds);
    };

    if (firstButton != nullptr && firstButton->isVisible()) {
        layoutRow(firstRow, firstButton.get());
    }

    if (addButton != nullptr) {
        layoutRow(secondRow, addButton.get());
    }
}

void ReverbComponent::paint(juce::Graphics& g) {
    SecondaryContainer::paint(g);

    auto contentBounds = getContainerBounds();
    auto listBounds = contentBounds.removeFromLeft(89).reduced(8, 10);
    const int rowHeight = 32;
    const int rowGap = 8;
    const int labelWidth = 18;

    g.setColour(juce::Colour::fromString("#2F2C3F"));
    g.setFont(juce::Font(12.0f));

    auto firstRow = listBounds.removeFromTop(rowHeight);
    listBounds.removeFromTop(rowGap);
    auto secondRow = listBounds.removeFromTop(rowHeight);

    if (firstButton != nullptr && firstButton->isVisible()) {
        auto labelBounds = firstRow.removeFromLeft(labelWidth);
        g.drawText("A:", labelBounds, juce::Justification::centredLeft, false);
    }

    auto labelBounds = secondRow.removeFromLeft(labelWidth);
    g.drawText("A:", labelBounds, juce::Justification::centredLeft, false);
}

void ReverbComponent::addSend(juce::String name) {
    reverbs.push_back(std::move(name));
    refreshButtons();
    resized();
    repaint();
}

void ReverbComponent::refreshButtons() {
    if (reverbs.empty()) {
        firstButton->setVisible(false);
    } else {
        firstButton->setVisible(true);
        firstButton->setState(RoundedButton::State::Active);
        firstButton->setLabel(reverbs.front());
    }

    addButton->setState(RoundedButton::State::Inactive);
    addButton->setLabel("+");
}
