#include "AutomationPanel.h"

AutomationPanel::AutomationPanel(const Edit* edit)
    : SecondaryContainer(edit),
      dialToggle("Dial"),
      writeButton("Write", RoundedButton::State::Inactive),
      punchButton("Punch", RoundedButton::State::Inactive),
      suspendButton("Suspend", RoundedButton::State::Inactive) {
    setTitle("Automation");

    readMode.setItems({ { "Read", 1 } });
    readMode.getComboBox().setSelectedId(1, juce::dontSendNotification);

    addAndMakeVisible(readMode);
    addAndMakeVisible(dialToggle);
    addAndMakeVisible(writeButton);
    addAndMakeVisible(punchButton);
    addAndMakeVisible(suspendButton);
}

void AutomationPanel::resized() {
    auto bounds = getContainerBounds().reduced(6, 6);
    const int gap = 6;
    const int leftWidth = 70;
    const auto rowHeight = (bounds.getHeight() - gap) / 2;

    auto leftColumn = bounds.removeFromLeft(leftWidth);
    bounds.removeFromLeft(gap);
    auto rightColumn = bounds;

    readMode.setBounds(leftColumn.withSizeKeepingCentre(leftColumn.getWidth(), rowHeight));

    const auto rightCellWidth = (rightColumn.getWidth() - gap) / 2;

    auto topRow = rightColumn.removeFromTop(rowHeight);

    rightColumn.removeFromTop(gap);
    auto bottomRow = rightColumn;

    auto dialBounds = topRow.removeFromLeft(rightCellWidth);

    topRow.removeFromLeft(gap);
    auto punchBounds = topRow;

    auto writeBounds = bottomRow.removeFromLeft(rightCellWidth);

    bottomRow.removeFromLeft(gap);
    auto suspendBounds = bottomRow;

    dialToggle.setBounds(dialBounds);
    punchButton.setBounds(punchBounds);
    writeButton.setBounds(writeBounds);
    suspendButton.setBounds(suspendBounds);
}
