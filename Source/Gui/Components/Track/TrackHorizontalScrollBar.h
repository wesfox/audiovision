#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"

/// Horizontal scroll bar for the timeline view range.
class TrackHorizontalScrollBar : public juce::Component,
                                 private juce::ValueTree::Listener {
public:
    explicit TrackHorizontalScrollBar(Edit& edit);
    ~TrackHorizontalScrollBar() override;

    int getPreferredHeight() const;

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

private:
    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override;

    int64 getTotalSamples() const;
    void setViewStartFromThumb(float thumbStart);

    Edit& edit;
    bool isDragging = false;
    float dragOffset = 0.0f;
    float lastThumbStart = 0.0f;
};
