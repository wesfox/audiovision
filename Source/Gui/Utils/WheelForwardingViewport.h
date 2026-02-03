#pragma once

#include <JuceHeader.h>

/// Viewport that forwards wheel gestures to a handler.
class WheelForwardingViewport : public juce::Viewport {
public:
    /// Handles wheel gestures and decides whether to consume them.
    class Handler {
    public:
        virtual ~Handler() = default;

        /// Handle a wheel gesture.
        /// @param event event context for the wheel gesture
        /// @param details wheel delta details
        /// @returns true when the gesture is consumed
        virtual bool handleWheel(const juce::MouseEvent& event,
                                 const juce::MouseWheelDetails& details) = 0;
    };

    /// Create a viewport with an optional wheel handler.
    /// @param handler handler used to consume wheel gestures
    explicit WheelForwardingViewport(Handler* handler = nullptr);

    /// Set the wheel handler.
    /// @param handler handler used to consume wheel gestures
    void setWheelHandler(Handler* handler);

    void mouseWheelMove(const juce::MouseEvent& event,
                        const juce::MouseWheelDetails& details) override;

private:
    Handler* handler = nullptr;
};
