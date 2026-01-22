#include "AudioTrack.h"

AudioTrack::AudioTrack(const String &name)
    : Track(name),
      inputMonitoring(false),
      armed(false)
      //recorder(Recorder())
{
    isAudioTrack_ = true;
    trackType = TrackType::Audio;
}
