#pragma once

#include <JuceHeader.h>

#include <array>

#include "Gui/Components/Common/SecondaryContainer.h"
#include "Gui/Components/Common/RotaryFader.h"
#include "Gui/Components/Common/ToggleTextButton.h"

/// Displays volume and pan controls.
class VolumeAndPan : public SecondaryContainer {
public:
    VolumeAndPan();
    ~VolumeAndPan() override;

    void resized() override;

private:
    class LabelledKnob;

    ToggleTextButton stereoLinkToggle;
    std::array<std::unique_ptr<LabelledKnob>, 6> knobs;
    std::unique_ptr<LabelledKnob> mainVolume;
};
