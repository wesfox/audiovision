#include "AudioTrack.h"

#include "Utils/IO/Recorder.h"

AudioTrack::AudioTrack()
    : Track(),
      inputMonitoring(false),
      armed(false),
      recorder(Recorder())
{
}