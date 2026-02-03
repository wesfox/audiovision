#pragma once

#include "Gui/Components/Common/ui/MultiStateToggleButton.h"

/// Track header toggle for record arm.
class ArmedToggle final : public MultiStateToggleButton {
public:
    enum class State {
        Inactive = 0,
        Active = 1
    };

    ArmedToggle();
};
