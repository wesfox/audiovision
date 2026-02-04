#pragma once

#include <JuceHeader.h>

#include "Core/Track/TrackState.h"

/// Edit view and global state backed by a ValueTree.
class EditState {
public:
    static const juce::Identifier kWaveformScaleId;

    /// Solo interaction mode for track commands.
    enum class SoloMode {
        Latch = 0,
        Xor = 1
    };

    /// Create a state tree with default values.
    EditState();

    /// Access the root ValueTree.
    juce::ValueTree& getRoot() { return root; }

    /// Access the root ValueTree (const).
    const juce::ValueTree& getRoot() const { return root; }

    /// Current view start sample.
    int64 getViewStartSample() const;

    /// Current view end sample.
    int64 getViewEndSample() const;

    /// Current frame rate.
    float getFrameRate() const;

    /// Set the visible sample range.
    /// @param startSample view start sample
    /// @param endSample view end sample
    /// @param undo optional undo manager for transactions
    void setViewRange(int64 startSample, int64 endSample, juce::UndoManager* undo = nullptr);

    /// Apply a zoom ratio to the current view.
    /// @param ratio zoom ratio (positive to zoom out, negative to zoom in)
    /// @param centerSample sample around which to center the view
    /// @param undo optional undo manager for transactions
    void zoom(float ratio, int64 centerSample, juce::UndoManager* undo = nullptr);

    /// Set the frame rate.
    /// @param frameRate new frame rate
    /// @param undo optional undo manager for transactions
    void setFrameRate(float frameRate, juce::UndoManager* undo = nullptr);

    /// Timeline ruler height in pixels.
    int getTimelineHeight() const;

    /// Set the timeline ruler height in pixels.
    /// @param height new ruler height in pixels
    /// @param undo optional undo manager for transactions
    void setTimelineHeight(int height, juce::UndoManager* undo = nullptr);

    /// Header height in pixels.
    int getHeaderHeight() const;

    /// Set the header height in pixels.
    /// @param height new header height in pixels
    /// @param undo optional undo manager for transactions
    void setHeaderHeight(int height, juce::UndoManager* undo = nullptr);

    /// Waveform vertical scale factor.
    float getWaveformScale() const;

    /// True when a selection range is active.
    bool hasSelectionRange() const;

    /// Selection range start sample.
    int64 getSelectionStartSample() const;

    /// Selection range end sample.
    int64 getSelectionEndSample() const;

    /// Current cursor sample.
    int64 getCursorSample() const;

    /// Current solo interaction mode.
    SoloMode getSoloMode() const;

    /// Set the waveform vertical scale factor.
    /// @param scale new waveform scale factor
    /// @param undo optional undo manager for transactions
    void setWaveformScale(float scale, juce::UndoManager* undo = nullptr);

    /// Set the selection range samples.
    /// @param startSample selection start sample
    /// @param endSample selection end sample
    /// @param undo optional undo manager for transactions
    void setSelectionRange(int64 startSample, int64 endSample, juce::UndoManager* undo = nullptr);

    /// Set the cursor sample.
    /// @param sample new cursor sample
    /// @param undo optional undo manager for transactions
    void setCursorSample(int64 sample, juce::UndoManager* undo = nullptr);

    /// Clear the active selection range.
    /// @param undo optional undo manager for transactions
    void clearSelectionRange(juce::UndoManager* undo = nullptr);

    /// True when the cursor follows playback on stop.
    bool getInsertionFollowsPlayback() const;

    /// True when looping playback is enabled.
    bool getIsLooping() const;

    /// Set whether the cursor follows playback on stop.
    /// @param followsPlayback new mode value
    /// @param undo optional undo manager for transactions
    void setInsertionFollowsPlayback(bool followsPlayback, juce::UndoManager* undo = nullptr);

    /// Set whether looping playback is enabled.
    /// @param isLooping new loop mode value
    /// @param undo optional undo manager for transactions
    void setIsLooping(bool isLooping, juce::UndoManager* undo = nullptr);

    /// Set the solo interaction mode.
    /// @param mode new solo mode
    /// @param undo optional undo manager for transactions
    void setSoloMode(SoloMode mode, juce::UndoManager* undo = nullptr);

    /// Convert a pixel position to a sample position in the current view.
    /// @param pixelX x position in pixels
    /// @param viewWidth width of the view in pixels
    int64 mapPixelToSample(float pixelX, float viewWidth) const;

    /// Ensure a track state node exists.
    /// @param trackId identifier of the track
    void ensureTrackState(const String& trackId);

    /// Track arm state.
    TrackArmState getTrackArmState(const String& trackId) const;

    /// Track input monitoring state.
    TrackInputMonitoringState getTrackInputMonitoringState(const String& trackId) const;

    /// Track solo state.
    TrackSoloState getTrackSoloState(const String& trackId) const;

    /// Track solo-safe flag.
    bool getTrackSoloSafeState(const String& trackId) const;

    /// Track mute state.
    TrackMuteState getTrackMuteState(const String& trackId) const;

    /// Track user mute state.
    TrackMuteState getTrackUserMuteState(const String& trackId) const;

    /// Set the arm state for a track.
    /// @param trackId identifier of the track
    /// @param state new arm state
    /// @param undo optional undo manager for transactions
    void setTrackArmState(const String& trackId, TrackArmState state, juce::UndoManager* undo = nullptr);

    /// Set the input monitoring state for a track.
    /// @param trackId identifier of the track
    /// @param state new input monitoring state
    /// @param undo optional undo manager for transactions
    void setTrackInputMonitoringState(const String& trackId, TrackInputMonitoringState state, juce::UndoManager* undo = nullptr);

    /// Set the solo state for a track.
    /// @param trackId identifier of the track
    /// @param state new solo state
    /// @param undo optional undo manager for transactions
    void setTrackSoloState(const String& trackId, TrackSoloState state, juce::UndoManager* undo = nullptr);

    /// Set the solo-safe flag for a track.
    /// @param trackId identifier of the track
    /// @param soloSafe new solo-safe flag
    /// @param undo optional undo manager for transactions
    void setTrackSoloSafeState(const String& trackId, bool soloSafe, juce::UndoManager* undo = nullptr);

    /// Set the mute state for a track.
    /// @param trackId identifier of the track
    /// @param state new mute state
    /// @param undo optional undo manager for transactions
    void setTrackMuteState(const String& trackId, TrackMuteState state, juce::UndoManager* undo = nullptr);

    /// Set the user mute state for a track.
    /// @param trackId identifier of the track
    /// @param state new user mute state
    /// @param undo optional undo manager for transactions
    void setTrackUserMuteState(const String& trackId, TrackMuteState state, juce::UndoManager* undo = nullptr);

    /// Access the state node for a track.
    /// @param trackId identifier of the track
    juce::ValueTree getTrackState(const String& trackId) const;

private:
    juce::ValueTree getOrCreateTrackState(const String& trackId);

    juce::ValueTree root;
    juce::ValueTree globals;
    juce::ValueTree viewState;
    juce::ValueTree trackState;
};
