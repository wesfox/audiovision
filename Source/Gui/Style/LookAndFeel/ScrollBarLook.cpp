#include "ScrollBarLook.h"

#include <algorithm>

ScrollBarLook::ScrollBarLook() {
    setColour(juce::ScrollBar::thumbColourId, juce::Colour::fromString("#FFF4F1FF"));
}

void ScrollBarLook::drawScrollbar(juce::Graphics& g,
                                  juce::ScrollBar& scrollbar,
                                  int x,
                                  int y,
                                  int width,
                                  int height,
                                  bool isScrollbarVertical,
                                  int thumbStartPosition,
                                  int thumbSize,
                                  bool isMouseOver,
                                  bool isMouseDown) {
    juce::ignoreUnused(scrollbar, isMouseOver, isMouseDown, y, height);
    if (thumbSize <= 0) {
        return;
    }
    const auto thumbColour = findColour(juce::ScrollBar::thumbColourId);
    g.setColour(thumbColour);
    const auto corner = static_cast<float>(std::min(width, thumbSize)) * 0.5f;
    if (isScrollbarVertical) {
        g.fillRoundedRectangle(static_cast<float>(x + padding),
                               static_cast<float>(thumbStartPosition),
                               static_cast<float>(width - padding),
                               static_cast<float>(thumbSize),
                               corner);
    } else {
        g.fillRoundedRectangle(static_cast<float>(thumbStartPosition),
                               static_cast<float>(y),
                               static_cast<float>(thumbSize),
                               static_cast<float>(height),
                               corner);
    }
}
