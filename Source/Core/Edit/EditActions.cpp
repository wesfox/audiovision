#include "EditActions.h"

EditAction EditAction::makeViewRange(int64 start, int64 end) {
    EditAction action;
    action.type = EditActionType::SetViewRange;
    action.startSample = start;
    action.endSample = end;
    return action;
}

EditAction EditAction::makeZoom(float ratio, int64 centerSample) {
    EditAction action;
    action.type = EditActionType::Zoom;
    action.zoomRatio = ratio;
    action.zoomCenterSample = centerSample;
    return action;
}

EditAction EditAction::makeFrameRate(float frameRate) {
    EditAction action;
    action.type = EditActionType::SetFrameRate;
    action.frameRate = frameRate;
    return action;
}

juce::String getEditActionName(EditActionType type) {
    switch (type) {
        case EditActionType::SetViewRange:
            return "Edit: Set View Range";
        case EditActionType::Zoom:
            return "Edit: Zoom";
        case EditActionType::SetFrameRate:
            return "Edit: Set Frame Rate";
        default:
            return "Edit: Action";
    }
}

EditActionStore::EditActionStore(EditState& state, juce::UndoManager& undoManager)
    : state(state), undoManager(undoManager) {
}

void EditActionStore::dispatch(const EditAction& action) {
    undoManager.beginNewTransaction(getEditActionName(action.type));
    applyAction(action);
    listeners.call([&action](EditActionListener& listener) { listener.editActionDispatched(action); });
}

void EditActionStore::addListener(EditActionListener* listener) {
    listeners.add(listener);
}

void EditActionStore::removeListener(EditActionListener* listener) {
    listeners.remove(listener);
}

void EditActionStore::applyAction(const EditAction& action) {
    switch (action.type) {
        case EditActionType::SetViewRange:
            state.setViewRange(action.startSample, action.endSample, nullptr);
            break;
        case EditActionType::Zoom:
            state.zoom(action.zoomRatio, action.zoomCenterSample, nullptr);
            break;
        case EditActionType::SetFrameRate:
            state.setFrameRate(action.frameRate, &undoManager);
            break;
        default:
            break;
    }
}
