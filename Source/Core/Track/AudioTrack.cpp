#include "AudioTrack.h"

#include "Utils/IO/Recorder.h"

AudioTrack::AudioTrack(const String &name)
    : Track(name),
      inputMonitoring(false),
      armed(false),
      recorder(Recorder())
{
}