#pragma once

#include <JuceHeader.h>

#include <vector>

#include "Gui/Components/Common/Fader.h"
#include "Gui/Components/Common/RoundedButton.h"
#include "Gui/Components/Common/SecondaryContainer.h"

/// Displays a reverb list with a dedicated fader.
class ReverbComponent : public SecondaryContainer {
public:
    explicit ReverbComponent(const Edit* edit = nullptr);

    void resized() override;
    void paint(juce::Graphics& g) override;

    void addSend(juce::String name);

private:
    void refreshButtons();

    std::vector<juce::String> reverbs;
    std::unique_ptr<RoundedButton> firstButton;
    std::unique_ptr<RoundedButton> addButton;
    Fader fader;
};
