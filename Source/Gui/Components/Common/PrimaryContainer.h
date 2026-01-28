#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"

/// Renders the primary header container with a left accent bar.
class PrimaryContainer : public juce::Component {
public:
    explicit PrimaryContainer(const Edit* edit = nullptr);

    void paint(juce::Graphics& g) override;

    void setTitle(juce::String newTitle);
    juce::Rectangle<int> getContainerBounds() const;

    static constexpr int kHeaderHeight = 32;

private:
    const Edit* edit = nullptr;
    juce::String title;
};
