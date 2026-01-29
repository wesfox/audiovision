#include "InsertsComponent.h"

InsertsComponent::InsertsComponent()
    : addButton("+") {
    setTitle("Inserts");
    addButton.setState(RoundedButton::State::Inactive);
    addButton.onClick = [this] {
      addInsert("New Insert");
    };
    addAndMakeVisible(addButton);
}

void InsertsComponent::addInsert(juce::String insertName) {
    inserts.push_back(std::move(insertName));
    refreshButtons();
    resized();
    repaint();
}

void InsertsComponent::resized() {
    auto contentBounds = getContainerBounds().reduced(12, 10);
    const int rowHeight = 32;
    const int rowGap = 8;
    const int labelWidth = 18;

    auto layoutRow = [&](juce::Rectangle<int> rowBounds, RoundedButton& button) {
        rowBounds.removeFromLeft(labelWidth);
        button.setBounds(rowBounds.withTrimmedLeft(2));
    };

    for (const auto& button : insertButtons) {
        auto rowBounds = contentBounds.removeFromTop(rowHeight);
        layoutRow(rowBounds, *button);
        contentBounds.removeFromTop(rowGap);
    }

    if (addButton.isVisible()) {
        auto rowBounds = contentBounds.removeFromTop(rowHeight);
        layoutRow(rowBounds, addButton);
    }
}

void InsertsComponent::paint(juce::Graphics& g) {
    SecondaryContainer::paint(g);

    auto contentBounds = getContainerBounds().reduced(12, 10);
    const int rowHeight = 32;
    const int rowGap = 8;
    const int labelWidth = 18;

    g.setColour(juce::Colour::fromString("#FF2F2C3F"));
    g.setFont(juce::Font(12.0f));

    const auto rowCount = static_cast<int>(insertButtons.size()) + 1;
    for (int index = 0; index < rowCount; ++index) {
        auto rowBounds = contentBounds.removeFromTop(rowHeight);
        auto labelBounds = rowBounds.removeFromLeft(labelWidth);
        g.drawText("A:", labelBounds, juce::Justification::centredLeft, false);
        contentBounds.removeFromTop(rowGap);
    }
}

void InsertsComponent::refreshButtons() {
    insertButtons.clear();

    for (const auto& insert : inserts) {
        auto button = std::make_unique<RoundedButton>(insert);
        button->setState(RoundedButton::State::Active);
        addAndMakeVisible(*button);
        insertButtons.emplace_back(std::move(button));
    }

    addButton.setLabel("+");
    addButton.setState(RoundedButton::State::Inactive);
}
