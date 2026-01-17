#pragma once

#include "Fade.h"

class FadeOut : public Fade {
public:
    FadeOut();

    void remove() override;

    /// Sample at which the fade stats, relative to the Clip
    int startSample;
};