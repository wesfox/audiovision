#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"
#include "Gui/Assets/SvgFactory.h"

/// Transport cursor overlay for the timeline.
class CursorTimeline : public juce::Component {
public:
    explicit CursorTimeline(const Edit& edit);

    void paint(juce::Graphics& g) override;
    bool hitTest(int x, int y) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

    /// Update the cursor fill colour.
    /// @param colour new fill colour
    void setColour(juce::Colour colour);

private:
    const Edit& edit;
    std::unique_ptr<juce::Drawable> cursorDrawable;
    juce::Colour fillColour = juce::Colour::fromRGB(76, 44, 126);
    int64 dragStartSample = 0;
};
