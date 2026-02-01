#pragma once

#include <JuceHeader.h>
#include <Utils/Transport.h>
#include "Scene.h"
#include <Core/Automation/AutomationManager.h>
#include <Core/Video/Video.h>
#include "EditActions.h"

/// Edit model containing tracks, scenes, and global media.
class Edit {
public:
    /// Create an empty edit.
    Edit();
    ~Edit() = default;

    /// Add a track to the edit.
    /// @param track track to add
    void addTrack(std::shared_ptr<Track> track) {
        tracks.push_back(std::move(track));
    }

    /// Mutable track list (edit scope).
    std::vector<std::shared_ptr<Track>>& getTracks() {
        return tracks;
    }

    /// Read-only track list (edit scope).
    const std::vector<std::shared_ptr<Track>>& getTracks() const {
        return tracks;
    }

    /// Current audio output track.
    std::weak_ptr<Track> getAudioOutputTrack() {
        return audioOutputTrack;
    }

    /// Current audio output track (const).
    std::weak_ptr<Track> getAudioOutputTrack() const {
        return audioOutputTrack;
    }

    /// Set the audio output track.
    /// @param audioOutputTrackToSet track to use as output
    std::weak_ptr<Track> setAudioOutputTrack(const std::shared_ptr<Track>& audioOutputTrackToSet) {
        return audioOutputTrack = audioOutputTrackToSet;
    }

    /// Add a scene to the edit.
    /// @param scene scene to add
    void addScene(std::shared_ptr<Scene> scene) {
        scenes.push_back(std::move(scene));
    }

    /// Mutable scene list (edit scope).
    std::vector<std::shared_ptr<Scene>>& getScenes() {
        return scenes;
    }

    /// Read-only scene list (edit scope).
    const std::vector<std::shared_ptr<Scene>>& getScenes() const {
        return scenes;
    }

    int64 getViewStartSample() const {
        return editState.getViewStartSample();
    }

    int64 getViewEndSample() const {
        return editState.getViewEndSample();
    }

    float getFrameRate() const {
        return editState.getFrameRate();
    }

    float getHeight() const {
        float totalHeight = 0.0f;
        for (const auto& track : tracks) {
            if (track) {
                totalHeight += track->getHeight();
            }
        }
        return totalHeight;
    }

    void zoom(float ratio) {
        const auto transport = getTransport();
        const auto cursorSample = transport ? transport->getCursorPosition() : getViewStartSample();
        actionStore.dispatch(EditAction::makeZoom(ratio, cursorSample));
    }

    /// Access the edit state.
    EditState& getState() {
        return editState;
    }

    /// Access the edit state (const).
    const EditState& getState() const {
        return editState;
    }

    /// Access the edit action store.
    EditActionStore& getActionStore() {
        return actionStore;
    }

    /// Access the edit action store (const).
    const EditActionStore& getActionStore() const {
        return actionStore;
    }

    /// Access the undo manager.
    juce::UndoManager& getUndoManager() {
        return undoManager;
    }

    /// Access the undo manager (const).
    const juce::UndoManager& getUndoManager() const {
        return undoManager;
    }


    /// Perform an undoable action as a new transaction.
    /// @param name transaction name for the undo history
    /// @param action undoable action (owned by the undo manager)
    void performUndoable(const juce::String& name, juce::UndoableAction* action) {
        undoManager.beginNewTransaction(name);
        undoManager.perform(action);
    }

    /// Access the shared transport.
    std::shared_ptr<Transport> getTransport() const {
        return transport;
    }

private:
    /// Shared transport for timeline state.
    std::shared_ptr<Transport> transport;
    std::vector<std::shared_ptr<Scene>> scenes;
    String projectName;
    AutomationManager automationManager;
    int64 videoStartFrame;
    Video video;
    int64 videoSyncOffset;
    std::vector<std::shared_ptr<Track>> tracks;
    std::weak_ptr<Track> audioOutputTrack;

    EditState editState;
    juce::UndoManager undoManager;
    EditActionStore actionStore;
};
