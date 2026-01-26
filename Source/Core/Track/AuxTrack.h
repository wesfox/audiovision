#pragma once

#include <JuceHeader.h>

#include "Track.h"

/// Aux track used for shared processing chains.
class AuxTrack : public Track  {
public:
    /// Create an aux track with an optional name.
    /// @param name display name
    AuxTrack(const String &name="");

    /// Create a shared aux track.
    /// @param name display name
    static std::shared_ptr<AuxTrack> create(const String& name="")
    {
        return std::make_shared<AuxTrack>(name);
    }
private:
};
