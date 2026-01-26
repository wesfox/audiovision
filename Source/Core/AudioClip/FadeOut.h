#pragma once

#include "Fade.h"

/// Fade-out segment for an audio clip.
class FadeOut : public Fade {
public:
    /// Create a disabled fade-out.
    FadeOut();

    /// Disable this fade-out.
    void remove() override;

    /// Sample at which the fade stats, relative to the Clip
    int startSample;
};
