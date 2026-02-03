#include "WheelForwardingViewport.h"

WheelForwardingViewport::WheelForwardingViewport(Handler* handler)
    : handler(handler) {
}

void WheelForwardingViewport::setWheelHandler(Handler* handlerToSet) {
    handler = handlerToSet;
}

void WheelForwardingViewport::mouseWheelMove(const juce::MouseEvent& event,
                                             const juce::MouseWheelDetails& details) {
    if (handler != nullptr && handler->handleWheel(event, details)) {
        return;
    }
    juce::Viewport::mouseWheelMove(event, details);
}
