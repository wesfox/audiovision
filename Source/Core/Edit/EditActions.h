#pragma once

#include <JuceHeader.h>
#include "EditState.h"

/// Typed edit action dispatched through the action store.
enum class EditActionType {
    SetViewRange,
    Zoom,
    SetFrameRate
};

/// Action payload for edit state updates.
struct EditAction {
    EditActionType type = EditActionType::SetViewRange;
    int64 startSample = 0;
    int64 endSample = 0;
    float zoomRatio = 0.0f;
    int64 zoomCenterSample = 0;
    float frameRate = 0.0f;

    /// Create a view range update action.
    static EditAction makeViewRange(int64 start, int64 end);


    /// Create a zoom action.
    /// @param ratio zoom ratio (positive to zoom out, negative to zoom in)
    /// @param centerSample sample around which to center the view
    static EditAction makeZoom(float ratio, int64 centerSample);


    /// Create a frame rate update action.
    static EditAction makeFrameRate(float frameRate);
};

/// Transaction label for an edit action.
juce::String getEditActionName(EditActionType type);

/// Listener notified when an edit action is dispatched.
class EditActionListener {
public:
    virtual ~EditActionListener() = default;
    virtual void editActionDispatched(const EditAction& action) = 0;
};

/// Action dispatcher that applies edit actions and notifies listeners.
class EditActionStore {
public:
    /// Create a store bound to the edit state and undo manager.
    /// @param state edit state to update
    /// @param undoManager undo manager used for transactions
    EditActionStore(EditState& state, juce::UndoManager& undoManager);

    /// Dispatch an action and apply it to the state.
    /// @param action edit action to apply
    void dispatch(const EditAction& action);


    /// Register a listener for dispatched actions.
    /// @param listener listener to add
    void addListener(EditActionListener* listener);


    /// Remove a listener.
    /// @param listener listener to remove
    void removeListener(EditActionListener* listener);

private:
    void applyAction(const EditAction& action);

    EditState& state;
    juce::UndoManager& undoManager;
    juce::ListenerList<EditActionListener> listeners;
};
