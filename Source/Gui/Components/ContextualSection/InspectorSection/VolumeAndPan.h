#pragma once

#include <JuceHeader.h>

#include <array>

#include "../../Common/Containers/SecondaryContainer.h"
#include "../../Common/tools/RotaryFader.h"
#include "../../Common/tools/VerticalFader.h"
#include "../../Common/ui/RectToggleButton.h"

/// Displays volume and pan controls.
class VolumeAndPan : public SecondaryContainer {
public:
    VolumeAndPan();
    ~VolumeAndPan() override;

    void resized() override;

    static constexpr int kMinHeight = 220;

private:
    class LabelledKnob;
    class MainVolumeControl;

    RectToggleButton stereoLinkToggle;
    std::array<std::unique_ptr<LabelledKnob>, 6> knobs;
    std::unique_ptr<MainVolumeControl> mainVolume;
};
