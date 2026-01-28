#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"
#include "Gui/Assets/SvgFactory.h"

/// Transport cursor overlay for the timeline.
class CursorTimeline : public juce::Component {
public:
    CursorTimeline(const Edit& edit, int rulerHeight);

    void paint(juce::Graphics& g) override;
    bool hitTest(int x, int y) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

    struct Callbacks {
        std::function<void(int64 previousSample, int64 newSample)> onPointerDown;
        std::function<void(int64 newSample)> onPointerDrag;
        std::function<void(int64 previousSample, int64 newSample, bool wasDrag)> onPointerUp;
    };

    /// Register callbacks for cursor interactions.
    /// @param callbacks callback bundle
    void setCallbacks(Callbacks callbacks);

    /// Update the cursor fill colour.
    /// @param colour new fill colour
    void setColour(juce::Colour colour);

    /// Update the ruler height used for hit-testing/drawing.
    /// @param height new ruler height in pixels
    void setRulerHeight(int height);

private:
    const Edit& edit;
    std::unique_ptr<juce::Drawable> cursorDrawable;
    juce::Colour fillColour = juce::Colour::fromRGB(76, 44, 126);
    Callbacks callbacks;
    bool isDragging = false;
    int64 pointerDownSample = 0;
    int rulerHeight = 0;
};
