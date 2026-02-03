#pragma once

/// Arm state for recording.
enum class TrackArmState {
    Inactive = 0,
    Active = 1
};

/// Input monitoring state.
enum class TrackInputState {
    Inactive = 0,
    Active = 1
};

/// Solo state for track routing.
enum class TrackSoloState {
    Inactive = 0,
    Solo = 1,
    SoloSafe = 2
};

/// Mute state for track output.
enum class TrackMuteState {
    Active = 0,
    SoloMute = 1,
    Mute = 2
};
