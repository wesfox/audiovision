#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"
#include "Core/Track/TrackState.h"

/// Dispatches track state changes into the edit state.
class TrackCommandManager {
public:
    /// Create a track command manager bound to the edit.
    /// @param edit edit owning the track state
    explicit TrackCommandManager(Edit& edit);

    /// Set the arm state for a track.
    /// @param trackId identifier of the track
    /// @param state arm state to apply
    void setArmState(const String& trackId, TrackArmState state);

    /// Toggle the arm state for a track.
    /// @param trackId identifier of the track
    void toggleArmState(const String& trackId);

    /// Set the input monitoring state for a track.
    /// @param trackId identifier of the track
    /// @param state input monitoring state to apply
    void setInputMonitoringState(const String& trackId, TrackInputMonitoringState state);

    /// Toggle the input monitoring state for a track.
    /// @param trackId identifier of the track
    void toggleInputMonitoringState(const String& trackId);

    /// Set the solo state for a track.
    /// @param trackId identifier of the track
    /// @param state solo state to apply
    void setSoloState(const String& trackId, TrackSoloState state);

    /// Toggle the solo state for a track.
    /// @param trackId identifier of the track
    void toggleSoloState(const String& trackId);

    /// Set the mute state for a track.
    /// @param trackId identifier of the track
    /// @param state mute state to apply
    void setMuteState(const String& trackId, TrackMuteState state);
    void setUserMuteState(const String& trackId, TrackMuteState state);

    /// Toggle the mute state for a track.
    /// @param trackId identifier of the track
    void toggleMuteState(const String& trackId);

private:
    void updateMuteStates();

    Edit& edit;
};
