#pragma once

#include <JuceHeader.h>

/// Edit view and global state backed by a ValueTree.
class EditState {
public:
    static const juce::Identifier kWaveformScaleId;

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

    /// Set the waveform vertical scale factor.
    /// @param scale new waveform scale factor
    /// @param undo optional undo manager for transactions
    void setWaveformScale(float scale, juce::UndoManager* undo = nullptr);

    /// True when the playhead follows playback on stop.
    bool getInsertionFollowsPlayback() const;

    /// Set whether the playhead follows playback on stop.
    /// @param followsPlayback new mode value
    /// @param undo optional undo manager for transactions
    void setInsertionFollowsPlayback(bool followsPlayback, juce::UndoManager* undo = nullptr);

    /// Convert a pixel position to a sample position in the current view.
    /// @param pixelX x position in pixels
    /// @param viewWidth width of the view in pixels
    int64 mapPixelToSample(float pixelX, float viewWidth) const;

private:
    juce::ValueTree root;
    juce::ValueTree globals;
    juce::ValueTree viewState;
};
