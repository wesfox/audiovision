#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"
#include "Core/Edit/EditState.h"
#include "TimelineRuler.h"

class TrackContentPanel : public juce::Component,
                          public juce::ValueTree::Listener,
                          private juce::AsyncUpdater {
public:
    explicit TrackContentPanel(Edit& edit);
    ~TrackContentPanel() override;
    void updateLayout();
    void resized() override;
    void paint(juce::Graphics& g) override;
    void valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& property) override;

private:
    void handleAsyncUpdate() override;

    std::unique_ptr<TimelineRuler> timelineRuler;

    Edit& edit;
    std::map<String, std::shared_ptr<class TrackContent>> trackContentComponents;
};
