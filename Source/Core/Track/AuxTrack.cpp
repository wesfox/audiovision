#include "AuxTrack.h"

// ------------------------ MainComponent Implementation ------------------------

AuxTrack::AuxTrack(const String &name): Track(name) {
    trackType = TrackType::Aux;
}
