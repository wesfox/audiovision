#include "AudioTrack.h"

AudioTrack::AudioTrack(const String &name)
    : Track(name)
      //recorder(Recorder())
{
    isAudioTrack_ = true;
    trackType = TrackType::Audio;
}
