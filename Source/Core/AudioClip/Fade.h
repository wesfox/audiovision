#pragma once

#include <JuceHeader.h>

class Fade {
public:
    Fade();
    virtual ~Fade() = default;

    virtual void remove() = 0;

protected:
    bool isActive = false;
};
