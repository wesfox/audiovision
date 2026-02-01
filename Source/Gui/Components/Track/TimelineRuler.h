#pragma once

#include <JuceHeader.h>
#include "Core/Edit/Edit.h"
#include "Gui/Utils/ViewRangeMapper.h"

/// Timeline ruler showing timecode based on edit state.
class TimelineRuler : public juce::Component {
public:
    TimelineRuler(const Edit& edit, int rulerHeight);

    void paint(juce::Graphics& g) override;

    /// Update the ruler height used for layout and drawing.
    /// @param height new ruler height in pixels
    void setRulerHeight(int height);

private:
    ViewRangeMapper getMapper() const;
    const Edit& edit;
    int rulerHeight = 0;
};
