#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"

/// Renders the secondary header container.
class SecondaryContainer : public juce::Component {
public:
    explicit SecondaryContainer(const Edit* edit = nullptr);

    void paint(juce::Graphics& g) override;

    void setTitle(juce::String newTitle);
    juce::Rectangle<int> getContainerBounds() const;

private:
    const Edit* edit = nullptr;
    juce::String title;
};
