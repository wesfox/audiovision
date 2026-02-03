#pragma once

#include "Core/Track/TrackState.h"
#include "Gui/Components/Common/ui/MultiStateToggleButton.h"

/// Track header toggle for solo states.
class SoloToggle final : public MultiStateToggleButton {
public:
    using State = TrackSoloState;

    SoloToggle();
};
