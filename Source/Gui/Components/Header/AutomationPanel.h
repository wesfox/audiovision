#pragma once

#include <JuceHeader.h>

#include "../Common/ui/RoundedButton.h"
#include "../Common/Containers/SecondaryContainer.h"
#include "../Common/ui/SelectableList.h"
#include "../Common/ui/RectToggleButton.h"

/// Displays automation controls inside a secondary container.
class AutomationPanel : public SecondaryContainer {
public:
    explicit AutomationPanel(const Edit* edit = nullptr);

    void resized() override;

    static constexpr int kWidth = 200;

private:
    SelectableList readMode;
    RectToggleButton dialToggle;
    RoundedButton writeButton;
    RoundedButton punchButton;
    RoundedButton suspendButton;
};
