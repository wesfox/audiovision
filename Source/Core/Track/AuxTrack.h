#pragma once

#include <JuceHeader.h>

#include "Track.h"

class AuxTrack : public Track  {
public:
    AuxTrack(const String &name="");

    /// This function returns a new AuxTrack
    static std::shared_ptr<AuxTrack> create(const String& name="")
    {
        return std::make_shared<AuxTrack>(name);
    }
private:
};
