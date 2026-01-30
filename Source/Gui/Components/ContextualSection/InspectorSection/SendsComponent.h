#pragma once

#include <JuceHeader.h>

#include <vector>

#include "../../Common/ui/RoundedButton.h"
#include "../../Common/tools/RotaryFader.h"
#include "../../Common/Containers/SecondaryContainer.h"

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
