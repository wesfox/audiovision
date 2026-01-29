#pragma once

#include <JuceHeader.h>

#include <vector>

#include "Gui/Components/Common/RoundedButton.h"
#include "Gui/Components/Common/RotaryFader.h"
#include "Gui/Components/Common/SecondaryContainer.h"

/// Displays rotary sends in a secondary container.
class SendsComponent : public SecondaryContainer {
public:
    SendsComponent();
    ~SendsComponent() override;

    void addSend(juce::String sendName);
    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    class SendKnob;

    void refreshKnobs();

    std::vector<juce::String> sends;
    std::vector<std::unique_ptr<SendKnob>> knobs;
    RoundedButton addButton;
};
