#pragma once

#include "Core/Track/TrackState.h"
#include "Gui/Components/Common/ui/MultiStateToggleButton.h"

/// Track header toggle for record arm.
class ArmedToggle final : public MultiStateToggleButton {
public:
    using State = TrackArmState;

    ArmedToggle();
};
