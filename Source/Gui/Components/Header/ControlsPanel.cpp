#include "ControlsPanel.h"

#include <algorithm>
#include <optional>

#include "../Common/ui/SvgButton.h"

class ControlsPanel::SvgButton : public ::SvgButton {
public:
    using ::SvgButton::SvgButton;
};

ControlsPanel::ControlsPanel(Edit& edit) {
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

    rewind->onClick = [&edit] { edit.getTransport()->rewind(); };

    play->onClick = [this, &edit] {
        if (const auto transport = edit.getTransport()) {
            int64_t startSample = edit.getState().getCursorSample();
            std::optional<int64_t> endSample;
            if (edit.getState().hasSelectionRange()) {
                const auto selectionStart = edit.getState().getSelectionStartSample();
                const auto selectionEnd = edit.getState().getSelectionEndSample();
                startSample = std::min(selectionStart, selectionEnd);
                const auto orderedEnd = std::max(selectionStart, selectionEnd);
                if (orderedEnd > startSample) {
                    endSample = std::max<int64_t>(0, orderedEnd - 1);
                }
                playSelectionStartSample = startSample;
            } else {
                playSelectionStartSample.reset();
            }
            edit.getState().setCursorSample(startSample);
            transport->setPlayheadSample(startSample);
            transport->play(endSample);
        }
    };

    stop->onClick = [this, &edit] {
        if (const auto transport = edit.getTransport()) {
            const auto playheadSample = transport->getPlayheadSample();
            const auto cursorSample = edit.getState().getCursorSample();
            const bool followsPlayback = edit.getState().getInsertionFollowsPlayback();
            transport->stop();
            if (playSelectionStartSample.has_value()
                && edit.getState().hasSelectionRange()
                && cursorSample == playSelectionStartSample.value()) {
                if (followsPlayback) {
                    edit.getState().setCursorSample(playheadSample);
                } else {
                    transport->setPlayheadSample(cursorSample);
                }
            }
            playSelectionStartSample.reset();
        }
    };

    record->onClick = [&edit] {
        if (const auto transport = edit.getTransport()) {
            transport->stop();
        }
    };

    int64 jumpSize = (edit.getViewEndSample() - edit.getViewStartSample())/4;

    moveLeft->onClick = [&edit, jumpSize] {
        if (const auto transport = edit.getTransport()) {
            const auto nextSample = std::max<int64_t>(0, edit.getState().getCursorSample() - jumpSize);
            edit.getState().setCursorSample(nextSample);
            if (!transport->isPlaying()) {
                transport->setPlayheadSample(nextSample);
            }
        }
    };

    moveRight->onClick = [&edit, jumpSize] {
        if (const auto transport = edit.getTransport()) {
            const auto nextSample = std::max<int64_t>(0, edit.getState().getCursorSample() + jumpSize);
            edit.getState().setCursorSample(nextSample);
            if (!transport->isPlaying()) {
                transport->setPlayheadSample(nextSample);
            }
        }
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
