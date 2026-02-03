#include "TrackCommandManager.h"

TrackCommandManager::TrackCommandManager(Edit& edit)
    : edit(edit) {
}

namespace {
TrackArmState getNextState(TrackArmState state) {
    return state == TrackArmState::Active ? TrackArmState::Inactive : TrackArmState::Active;
}

TrackInputMonitoringState getNextInputMonitoringState(TrackInputMonitoringState state) {
    return state == TrackInputMonitoringState::Active ? TrackInputMonitoringState::Inactive : TrackInputMonitoringState::Active;
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
    return state == TrackMuteState::Mute ? TrackMuteState::Active : TrackMuteState::Mute;
}
}

void TrackCommandManager::setArmState(const String& trackId, TrackArmState state) {
    edit.getState().setTrackArmState(trackId, state, &edit.getUndoManager());
}

void TrackCommandManager::toggleArmState(const String& trackId) {
    const auto current = edit.getState().getTrackArmState(trackId);
    setArmState(trackId, getNextState(current));
}

void TrackCommandManager::setInputMonitoringState(const String& trackId, TrackInputMonitoringState state) {
    edit.getState().setTrackInputMonitoringState(trackId, state, &edit.getUndoManager());
}

void TrackCommandManager::toggleInputMonitoringState(const String& trackId) {
    const auto current = edit.getState().getTrackInputMonitoringState(trackId);
    setInputMonitoringState(trackId, getNextInputMonitoringState(current));
}

void TrackCommandManager::setSoloState(const String& trackId, TrackSoloState state) {
    edit.getState().setTrackSoloState(trackId, state, &edit.getUndoManager());
    updateMuteStates();
}

void TrackCommandManager::toggleSoloState(const String& trackId) {
    const auto current = edit.getState().getTrackSoloState(trackId);
    setSoloState(trackId, getNextState(current));
}

void TrackCommandManager::setMuteState(const String& trackId, TrackMuteState state) {
    edit.getState().setTrackMuteState(trackId, state, &edit.getUndoManager());
}

void TrackCommandManager::setUserMuteState(const String& trackId, TrackMuteState state) {
    edit.getState().setTrackUserMuteState(trackId, state, &edit.getUndoManager());
    updateMuteStates();
}

void TrackCommandManager::toggleMuteState(const String& trackId) {
    const auto current = edit.getState().getTrackUserMuteState(trackId);
    setUserMuteState(trackId, getNextState(current));
}

void TrackCommandManager::updateMuteStates() {
    const auto& tracks = edit.getTracks();
    bool anySolo = false;
    for (const auto& track : tracks) {
        if (!track) {
            continue;
        }
        const auto soloState = edit.getState().getTrackSoloState(track->getId());
        if (soloState == TrackSoloState::Solo) {
            anySolo = true;
            break;
        }
    }

    for (const auto& track : tracks) {
        if (!track) {
            continue;
        }
        const auto trackId = track->getId();
        const auto soloState = edit.getState().getTrackSoloState(trackId);
        const auto userMuteState = edit.getState().getTrackUserMuteState(trackId);
        const bool isSolo = soloState == TrackSoloState::Solo;
        const bool isSoloSafe = soloState == TrackSoloState::SoloSafe;
        const bool impliedMute = anySolo && !isSolo && !isSoloSafe;
        TrackMuteState effectiveState = TrackMuteState::Active;
        if (userMuteState == TrackMuteState::Mute) {
            effectiveState = TrackMuteState::Mute;
        } else if (impliedMute) {
            effectiveState = TrackMuteState::SoloMute;
        }
        setMuteState(trackId, effectiveState);
    }
}
