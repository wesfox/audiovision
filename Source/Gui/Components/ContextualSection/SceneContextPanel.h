#pragma once

#include <JuceHeader.h>

#include <vector>

#include "../Common/Containers/PrimaryContainer.h"
#include "../Common/ui/RoundedButton.h"

/// Displays scene context information in the contextual section.
class SceneContextPanel : public PrimaryContainer {
public:
    SceneContextPanel();

    void resized() override;
    void paint(juce::Graphics& g) override;

    static constexpr int kHeight = 126;
    static constexpr int kButtonRowHeight = 25;

private:
    juce::String description = "INT. NICOLAS' BEDROOM - DAY";
    juce::String charactersLabel = "Characters:";
    std::vector<std::unique_ptr<RoundedButton>> buttons;
};
