#pragma once

#include "Fade.h"

/// Fade-in segment for an audio clip.
class FadeIn : public Fade {
public:
    /// Create a disabled fade-in.
    FadeIn();

    /// Disable this fade-in.
    void remove() override;

    /// Sample at which the fade ends, relative to the Clip
    int endSample;
};
