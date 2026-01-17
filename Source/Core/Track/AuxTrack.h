#pragma once

#include <JuceHeader.h>

#include "Track.h"

class AuxTrack : public Track  {
public:
    AuxTrack();

    /// This function returns a new AuxTrack
    static std::shared_ptr<AuxTrack> create()
    {
        return std::make_shared<AuxTrack>();
    }
private:
};
