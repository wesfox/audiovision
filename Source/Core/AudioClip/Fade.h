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

protected:
    /// True when the fade is active.
    bool isActive = false;
};
