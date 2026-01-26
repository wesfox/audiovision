#pragma once

#include <JuceHeader.h>
#include <Utils/Format.h>

#include "Utils/Volume.h"
#include "Utils/Pan/Pan.h"

class Track;

/// Send position relative to the track fader.
enum class SendType {
    PreFader,
    PostFader
};

/// Send routing to another track with gain/pan controls.
class Send {
public:
    /// Create a send to a destination track.
    /// @param destinationTrack target track
    /// @param name display name
    Send(const std::weak_ptr<Track> &destinationTrack, String  name = "");

    /// Create a send to a destination track.
    /// @param destinationTrack target track
    /// @param name display name
    static std::unique_ptr<Send> create(const std::shared_ptr<Track>& destinationTrack, const String& name="")
    {
        return std::make_unique<Send>(std::weak_ptr{destinationTrack}, name);
    }

    /// Destination track (may be empty).
    std::weak_ptr<Track> getDestinationTrack() {
        return destinationTrack;
    }

    /// Channel format derived from the destination track.
    ChannelsFormat getFormat() const;

private:
    String name;
    Volume volume;
    Pan pan;
    ChannelsFormat format;
    SendType type;
    std::weak_ptr<Track> destinationTrack;
};
