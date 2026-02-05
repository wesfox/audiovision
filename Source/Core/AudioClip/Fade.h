#pragma once

#include <JuceHeader.h>

/// Base class for clip fades.
class Fade {
public:
    /// Create an inactive fade.
    Fade();
    virtual ~Fade() = default;

    /// Disable this fade.
    virtual void remove() = 0;

    /// True when the fade is active.
    bool isActive() const {
        return active;
    }

    /// Enable or disable this fade.
    /// @param isEnabled new active state
    void setActive(bool isEnabled) {
        active = isEnabled;
    }

protected:
    /// True when the fade is active.
    bool active = false;
};
