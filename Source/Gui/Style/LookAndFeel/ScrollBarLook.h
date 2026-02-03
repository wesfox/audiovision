#pragma once

#include <JuceHeader.h>

/// Scrollbar look and feel for track viewport.
class ScrollBarLook : public juce::LookAndFeel_V4 {
public:
    ScrollBarLook();

    static constexpr int padding = 2;

    void drawScrollbar(juce::Graphics& g,
                       juce::ScrollBar& scrollbar,
                       int x,
                       int y,
                       int width,
                       int height,
                       bool isScrollbarVertical,
                       int thumbStartPosition,
                       int thumbSize,
                       bool isMouseOver,
                       bool isMouseDown) override;
};
