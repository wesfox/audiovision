#pragma once

#include "Core/Track/TrackState.h"
#include "Gui/Components/Common/ui/MultiStateToggleButton.h"

/// Track header toggle for mute states.
class MuteToggle final : public MultiStateToggleButton {
public:
    using State = TrackMuteState;

    MuteToggle();
};
