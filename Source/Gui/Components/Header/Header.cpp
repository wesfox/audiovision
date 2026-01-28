#include "Header.h"

#include "Gui/Style/Font.h"

Header::Header(const Edit& edit) : edit(edit) {
    addAndMakeVisible(controlsPanel);
    addAndMakeVisible(timecodesDisplay);
    addAndMakeVisible(globalVuMeter);
    addAndMakeVisible(automationPanel);
    addAndMakeVisible(trackFolderActivationPanel);
    automationPanel.setTitle("Automation");
}

void Header::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds();
    // background
    g.setColour(juce::Colour::fromString("#FFF2F0FD"));
    g.fillRoundedRectangle(bounds.toFloat(), 6.0f);

    // underline
    g.setColour(juce::Colour(0xFFC8A5FF));
    g.drawLine(0,bounds.getHeight(),bounds.getWidth(),bounds.getHeight());

    constexpr int padding = 8;

    // apply a first left padding
    bounds.removeFromLeft(padding);

    constexpr int barWidth = 8;
    constexpr int barRadius = 4;
    auto barBounds = bounds.removeFromLeft(barWidth);
    auto barRect = barBounds.withSizeKeepingCentre(barWidth, bounds.getHeight()-padding*2).toFloat();

    g.setColour(juce::Colour(0xFF4C2C7E));
    g.fillRoundedRectangle(barRect, static_cast<float>(barRadius));

    auto textBounds = bounds.withTrimmedTop(padding)
        .withTrimmedLeft(padding).withWidth(150);
    g.setColour(juce::Colour(0xFF2F2C3F));

    g.setFont(Fonts::p(24.0f, Fonts::Weight::Bold));
    g.drawText("Project name", textBounds.removeFromTop(30), juce::Justification::centredLeft, false);

    g.setFont(Fonts::p(18.0f, Fonts::Weight::Regular));
    g.drawText("Scene edit", textBounds.removeFromTop(24), juce::Justification::centredLeft, false);
}

void Header::resized() {
    auto bounds = getLocalBounds().withTrimmedLeft(180);
    auto controlsBounds = bounds.removeFromLeft(ControlsPanel::kWidth);
    controlsPanel.setBounds(controlsBounds.withSizeKeepingCentre(ControlsPanel::kWidth,
                                                                 ControlsPanel::kHeight));

    bounds.removeFromLeft(12);
    auto timecodeBounds = bounds.removeFromLeft(TimecodesDisplay::kWidth);
    timecodesDisplay.setBounds(timecodeBounds.withSizeKeepingCentre(TimecodesDisplay::kWidth,
                                                                    TimecodesDisplay::kHeight));

    bounds.removeFromLeft(12);
    auto secondaryBounds = bounds.removeFromLeft(90).reduced(0, 4);
    globalVuMeter.setBounds(secondaryBounds);

    bounds.removeFromLeft(12);
    auto automationBounds = bounds.removeFromLeft(AutomationPanel::kWidth).reduced(0, 4);
    automationPanel.setBounds(automationBounds);

    bounds.removeFromLeft(12);
    auto groupsBounds = bounds.removeFromLeft(TrackFolderActivationPanel::kWidth).reduced(0, 4);
    trackFolderActivationPanel.setBounds(groupsBounds);

}
