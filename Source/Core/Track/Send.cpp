#include "Send.h"

#include <utility>
#include "Track.h"

Send::Send(const std::weak_ptr<Track> &destinationTrack, String  name)
    : name(std::move(name)),
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
