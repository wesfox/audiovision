#pragma once

#include "Gui/Components/Common/ui/MultiStateToggleButton.h"

/// Track header toggle for mute states.
class MuteToggle final : public MultiStateToggleButton {
public:
    enum class State {
        Active = 0,
        SoloMute = 1,
        Mute = 2
    };

    MuteToggle();
};
