#include "Send.h"
#include "Track.h"

Send::Send(const std::weak_ptr<Track> &destinationTrack)
    : name("Send"),
      volume(Volume(1.0)),
      pan(Pan()),
      format(ChannelsFormat::Mono),
      type(SendType::PreFader),
      destinationTrack(destinationTrack)
{
}

ChannelsFormat Send::getFormat() const {
    return format;
}
