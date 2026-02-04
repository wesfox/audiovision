#pragma once

#include <JuceHeader.h>

#include <vector>

#include "Core/Edit/Edit.h"
#include "Core/Track/TrackState.h"

class FolderTrack;

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

    /// Apply a requested solo state with folder-aware rules.
    /// @param trackId identifier of the track
    /// @param requestedState desired solo state
    void requestSoloState(const String& trackId, TrackSoloState requestedState);

    /// Toggle the solo state for a track.
    /// @param trackId identifier of the track
    void toggleSoloState(const String& trackId);

    /// Toggle the solo-safe state for a track.
    /// @param trackId identifier of the track
    void toggleSoloSafeState(const String& trackId);

    /// Toggle solo for a selection of tracks.
    /// @param trackIds selected track identifiers
    void toggleSoloSelection(const std::vector<String>& trackIds);

    /// Toggle mute for a selection of tracks.
    /// @param trackIds selected track identifiers
    void toggleMuteSelection(const std::vector<String>& trackIds);

    /// Clear all solo states.
    void clearAllSolo();

    /// Clear all solo states and solo the requested track.
    /// @param trackId identifier of the track
    void clearAndSolo(const String& trackId);

    /// Set the mute state for a track.
    /// @param trackId identifier of the track
    /// @param state mute state to apply
    void setMuteState(const String& trackId, TrackMuteState state);
    void setUserMuteState(const String& trackId, TrackMuteState state);

    /// Toggle the mute state for a track.
    /// @param trackId identifier of the track
    void toggleMuteState(const String& trackId);

private:
    void applySoloOnAudio(const std::shared_ptr<Track>& track, bool enable);
    void applySoloOnFolder(const std::shared_ptr<FolderTrack>& folder, bool enable);
    bool hasActiveSoloInFolder(const FolderTrack& folder) const;
    void setFolderAndDescendantsSolo(FolderTrack& folder, TrackSoloState state);
    void setFolderAndDescendantsSoloDisabled(FolderTrack& folder);
    TrackSoloState getDisabledSoloState(const String& trackId) const;
    void setSoloStateInternal(const String& trackId, TrackSoloState state);
    std::shared_ptr<Track> getTrackById(const String& trackId) const;
    void clearAllSoloStates();
    void updateMuteStates();

    Edit& edit;
};
