#include "TrackCommandManager.h"

TrackCommandManager::TrackCommandManager(Edit& edit)
    : edit(edit) {
}

namespace {
TrackArmState getNextState(TrackArmState state) {
    return state == TrackArmState::Active ? TrackArmState::Inactive : TrackArmState::Active;
}

TrackInputState getNextState(TrackInputState state) {
    return state == TrackInputState::Active ? TrackInputState::Inactive : TrackInputState::Active;
}

TrackSoloState getNextState(TrackSoloState state) {
    switch (state) {
        case TrackSoloState::Inactive:
            return TrackSoloState::Solo;
        case TrackSoloState::Solo:
            return TrackSoloState::SoloSafe;
        case TrackSoloState::SoloSafe:
            return TrackSoloState::Inactive;
        default:
            return TrackSoloState::Inactive;
    }
}

TrackMuteState getNextState(TrackMuteState state) {
    switch (state) {
        case TrackMuteState::Active:
            return TrackMuteState::SoloMute;
        case TrackMuteState::SoloMute:
            return TrackMuteState::Mute;
        case TrackMuteState::Mute:
            return TrackMuteState::Active;
        default:
            return TrackMuteState::Active;
    }
}
}

void TrackCommandManager::setArmState(const String& trackId, TrackArmState state) {
    edit.getState().setTrackArmState(trackId, state, &edit.getUndoManager());
}

void TrackCommandManager::toggleArmState(const String& trackId) {
    const auto current = edit.getState().getTrackArmState(trackId);
    setArmState(trackId, getNextState(current));
}

void TrackCommandManager::setInputState(const String& trackId, TrackInputState state) {
    edit.getState().setTrackInputState(trackId, state, &edit.getUndoManager());
}

void TrackCommandManager::toggleInputState(const String& trackId) {
    const auto current = edit.getState().getTrackInputState(trackId);
    setInputState(trackId, getNextState(current));
}

void TrackCommandManager::setSoloState(const String& trackId, TrackSoloState state) {
    edit.getState().setTrackSoloState(trackId, state, &edit.getUndoManager());
}

void TrackCommandManager::toggleSoloState(const String& trackId) {
    const auto current = edit.getState().getTrackSoloState(trackId);
    setSoloState(trackId, getNextState(current));
}

void TrackCommandManager::setMuteState(const String& trackId, TrackMuteState state) {
    edit.getState().setTrackMuteState(trackId, state, &edit.getUndoManager());
}

void TrackCommandManager::toggleMuteState(const String& trackId) {
    const auto current = edit.getState().getTrackMuteState(trackId);
    setMuteState(trackId, getNextState(current));
}
