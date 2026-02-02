#pragma once

#include <JuceHeader.h>

#include "Utils/DebugWatchRegistry.h"

/// Renders debug watch values in a floating window.
class DebugWatchWindow : public juce::DocumentWindow {
public:
    DebugWatchWindow();
    ~DebugWatchWindow() override = default;

    void closeButtonPressed() override;

private:
    class DebugWatchView;
};
