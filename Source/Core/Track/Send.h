#pragma once

#include <JuceHeader.h>
#include <Utils/Format.h>

#include "Utils/Volume.h"
#include "Utils/Pan/Pan.h"

class Track;

enum class SendType {
    PreFader,
    PostFader
};

class Send {
public:
    Send(const std::weak_ptr<Track> &destinationTrack, String  name = "");

    /// This function returns a new Send
    static std::unique_ptr<Send> create(const std::shared_ptr<Track>& destinationTrack, const String& name="")
    {
        return std::make_unique<Send>(std::weak_ptr{destinationTrack}, name);
    }

    std::weak_ptr<Track> getDestinationTrack() {
        return destinationTrack;
    }

    /// Format of the "send", it is the format of the destinationTrack
    ChannelsFormat getFormat() const;

private:
    String name;
    Volume volume;
    Pan pan;
    ChannelsFormat format;
    SendType type;
    std::weak_ptr<Track> destinationTrack;
};
