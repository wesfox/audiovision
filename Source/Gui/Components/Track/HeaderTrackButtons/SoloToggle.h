#pragma once

#include "Gui/Components/Common/ui/MultiStateToggleButton.h"

/// Track header toggle for solo states.
class SoloToggle final : public MultiStateToggleButton {
public:
    enum class State {
        Inactive = 0,
        Solo = 1,
        SoloSafe = 2
    };

    SoloToggle();
};
