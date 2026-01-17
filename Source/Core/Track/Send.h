#pragma once

#include <JuceHeader.h>
#include <Utils/Format.h>

#include "Utils/Volume.h"
#include "Utils/Pan/Pan.h"

enum class SendType {
    PreFader,
    PostFader
};

class Send {
public:
    Send(const std::weak_ptr<Track> &destinationTrack);

    /// This function returns a new Send
    static std::unique_ptr<Send> create(std::weak_ptr<Track> destinationTrack)
    {
        return std::make_unique<Send>(destinationTrack);
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
