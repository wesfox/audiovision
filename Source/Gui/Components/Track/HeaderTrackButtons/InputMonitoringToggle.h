#pragma once

#include "Core/Track/TrackState.h"
#include "Gui/Components/Common/ui/MultiStateToggleButton.h"

/// Track header toggle for input monitoring.
class InputMonitoringToggle final : public MultiStateToggleButton {
public:
    using State = TrackInputState;

    InputMonitoringToggle();
};
