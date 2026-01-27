#pragma once

#include <JuceHeader.h>

/// Edit view and global state backed by a ValueTree.
class EditState {
public:
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
    /// @param undo optional undo manager for transactions
    void zoom(float ratio, juce::UndoManager* undo = nullptr);

    /// Set the frame rate.
    /// @param frameRate new frame rate
    /// @param undo optional undo manager for transactions
    void setFrameRate(float frameRate, juce::UndoManager* undo = nullptr);

private:
    juce::ValueTree root;
    juce::ValueTree globals;
    juce::ValueTree viewState;
};
