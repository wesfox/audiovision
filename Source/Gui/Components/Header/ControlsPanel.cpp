#include "ControlsPanel.h"

#include "Gui/Components/Common/SvgButton.h"

class ControlsPanel::SvgButton : public ::SvgButton {
public:
    using ::SvgButton::SvgButton;
};

ControlsPanel::ControlsPanel() {
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

    for (auto* icon : { rewind.get(), play.get(), stop.get(), record.get(), moveLeft.get(), moveRight.get() }) {
        addAndMakeVisible(icon);
    }

    addMouseListener(this, true);
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

void ControlsPanel::mouseUp(const juce::MouseEvent& event) {
    auto* component = event.originalComponent;
    if (component == nullptr || component == this) {
        return;
    }
    juce::Logger::writeToLog(component->getName());
}

void ControlsPanel::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat().reduced(0.5f);
    g.setColour(juce::Colour::fromString("#FFC8A5FF"));
    g.drawRoundedRectangle(bounds.toFloat(), 6.0, 1);
}
