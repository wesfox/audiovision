#pragma once

#include <JuceHeader.h>

/// Displays the transport control icons in the header.
class ControlsPanel : public juce::Component {
public:
    ControlsPanel();
    ~ControlsPanel() override;

    void paint(juce::Graphics& g) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void resized() override;

    static constexpr int kIconSize = 30;
    static constexpr int kGap = 8;
    static constexpr int kIconCount = 6;
    static constexpr int kGridWidth = kIconCount * kIconSize + (kIconCount - 1) * kGap;
    static constexpr int kGridHeight = kIconSize;
    static constexpr int kPadding = kGap * 2;
    static constexpr int kWidth = kGridWidth + kPadding * 2;
    static constexpr int kHeight = kIconSize + kPadding * 2;

private:
    class SvgButton;

    std::unique_ptr<SvgButton> moveLeft;
    std::unique_ptr<SvgButton> moveRight;
    std::unique_ptr<SvgButton> play;
    std::unique_ptr<SvgButton> record;
    std::unique_ptr<SvgButton> rewind;
    std::unique_ptr<SvgButton> stop;
};
