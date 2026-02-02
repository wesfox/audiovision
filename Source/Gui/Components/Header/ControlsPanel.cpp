#include "ControlsPanel.h"

#include <algorithm>
#include <optional>

#include "../Common/ui/SvgButton.h"
#include "Command/Commands.h"

class ControlsPanel::SvgButton : public ::SvgButton {
public:
    using ::SvgButton::SvgButton;
};

ControlsPanel::ControlsPanel(Edit& edit, juce::ApplicationCommandManager& commandManager)
    : commandManager(commandManager) {
    const auto iconColour = juce::Colour::fromString("#4C2C7E");

    rewind = std::make_unique<SvgButton>(SVG_Assets::RewindSvg, iconColour);
    play = std::make_unique<SvgButton>(SVG_Assets::PlaySvg, iconColour);
    stop = std::make_unique<SvgButton>(SVG_Assets::StopSvg, iconColour);
    record = std::make_unique<SvgButton>(SVG_Assets::RecordSvg, iconColour);
    moveLeft = std::make_unique<SvgButton>(SVG_Assets::MoveLeftSvg, iconColour);
    moveRight = std::make_unique<SvgButton>(SVG_Assets::MoveRightSvg, iconColour);

    rewind->setName("rewind");
    play->setName("play");
    stop->setName("stop");
    record->setName("record");
    moveLeft->setName("move-left");
    moveRight->setName("move-right");

    rewind->onClick = [&edit] {  };

    play->onClick = [this, &commandManager] {
        commandManager.invokeDirectly(CommandIds::Transport::playPause, true);
    };

    stop->onClick = [this, &edit] {
        
    };

    record->onClick = [&edit] {

    };

    int64 jumpSize = (edit.getViewEndSample() - edit.getViewStartSample())/4;

    moveLeft->onClick = [&edit, jumpSize] {

    };

    moveRight->onClick = [&edit, jumpSize] {

    };

    for (auto* icon : { rewind.get(), play.get(), stop.get(), record.get(), moveLeft.get(), moveRight.get() }) {
        addAndMakeVisible(icon);
    }
}

ControlsPanel::~ControlsPanel() = default;

void ControlsPanel::resized() {
    juce::Grid grid;
    grid.templateRows = { 1_fr };
    grid.templateColumns = { 1_fr, 1_fr, 1_fr, 1_fr, 1_fr, 1_fr };
    grid.setGap(juce::Grid::Px(kGap));

    for (auto* icon : { rewind.get(), play.get(), stop.get(), record.get(), moveLeft.get(), moveRight.get() }) {
        grid.items.add(juce::GridItem(*icon));
    }

    grid.performLayout(getLocalBounds().withSizeKeepingCentre(kGridWidth, kGridHeight));
}

void ControlsPanel::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat().reduced(0.5f);
    g.setColour(juce::Colour::fromString("#FFC8A5FF"));
    g.drawRoundedRectangle(bounds.toFloat(), 6.0, 1);
}
