#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"

class TrackContentPanel : public juce::Component {
public:
    explicit TrackContentPanel(const Edit& edit);
    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    const Edit& edit;
    std::map<String, std::shared_ptr<class TrackContent>> trackContentComponents;
};
