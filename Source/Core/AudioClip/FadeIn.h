#pragma once

#include "Fade.h"

class FadeIn : public Fade {
public:
    FadeIn();

    void remove() override;

    /// Sample at which the fade ends, relative to the Clip
    int endSample;
};
