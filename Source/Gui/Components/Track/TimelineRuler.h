#pragma once

#include <JuceHeader.h>
#include "Core/Edit/Edit.h"

/// Timeline ruler showing timecode based on edit state.
class TimelineRuler : public juce::Component {
public:
    explicit TimelineRuler(const Edit& edit);

    void paint(juce::Graphics& g) override;

private:
    const Edit& edit;
};
