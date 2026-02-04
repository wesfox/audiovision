#include "RecordTrack.h"

RecordTrack::RecordTrack(const String& name)
    : AudioTrack(name)
{
    trackType = TrackType::Record;
}
