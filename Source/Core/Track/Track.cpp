#include "Track.h"
#include <Utils/Uuid.h>

#include <utility>
#include "Send.h"

Track::Track(String name)
    : id(uuid::generate_uuid_v4()),
      outputTrack({}),
      inputTrack({}),
      automationMode(0),
      solo(false),
      soloSafe(false),
      mute(false),
      format(ChannelsFormat::Stereo),
      outputFormat(ChannelsFormat::Stereo),
      height(100.0f),
      name(std::move(name)),
      colour(juce::Colour(0xFF63A129))
{
}

Track::~Track() = default;

void Track::addSend(std::unique_ptr<Send> send)
{
    sends.push_back(std::move(send));
}

const std::vector<std::unique_ptr<Send>>& Track::getSends()
{
    return sends;
}

void Track::setOutput(std::weak_ptr<Track> newOutputTrack) {
    outputTrack = newOutputTrack;
}
