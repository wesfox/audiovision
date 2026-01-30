#pragma once

#include <JuceHeader.h>

#include <vector>

#include "../../Common/Containers/SecondaryContainer.h"
#include "../../Common/ui/RoundedButton.h"

/// Displays the inserts section.
class InsertsComponent : public SecondaryContainer {
public:
    InsertsComponent();

    void addInsert(juce::String insertName);
    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    void refreshButtons();

    std::vector<juce::String> inserts;
    std::vector<std::unique_ptr<RoundedButton>> insertButtons;
    RoundedButton addButton;
};
