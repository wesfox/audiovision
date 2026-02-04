#pragma once

#include "Core/Track/TrackState.h"
#include "Gui/Components/Common/ui/MultiStateToggleButton.h"

/// Track header toggle for solo states.
class SoloToggle final : public MultiStateToggleButton {
public:
    using State = TrackSoloState;
    using Callback = std::function<void()>;

    SoloToggle();

    /// Register a callback for unmodified click requests.
    /// @param callback function invoked on click without modifiers
    void onSoloRequested(Callback callback);

    /// Register a callback for command-click solo-safe requests.
    /// @param callback function invoked on command-click
    void onSoloSafeRequested(Callback callback);

    void mouseDown(const juce::MouseEvent& e) override;

private:
    Callback onSoloRequestedCallback;
    Callback onSoloSafeRequestedCallback;
};
