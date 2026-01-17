#pragma once

#include <JuceHeader.h>

enum class Channel
{
    Mono,

    L, R, C,
    LFE,

    Ls, Rs,
    Lb, Rb,

    Lw, Rw,

    Ltf, Rtf,
    Ltm, Rtm,
    Ltr, Rtr
};