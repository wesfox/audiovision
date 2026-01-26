#pragma once

#include <JuceHeader.h>
#include <Utils/Transport.h>
#include "Scene.h"
#include <Core/Automation/AutomationManager.h>
#include <Core/Video/Video.h>

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
        return viewStartSample;
    }

    int64 getViewEndSample() const {
        return viewEndSample;
    }

    int64 getFrameRate() const {
        return frameRate;
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
        viewEndSample = viewEndSample + static_cast<float>(viewEndSample - viewStartSample) * ratio;
        viewStartSample = viewStartSample + static_cast<float>(viewEndSample - viewStartSample) * ratio;
        viewStartSample = std::max(0LL, viewStartSample);
    }


private:
    Transport transport;
    std::vector<std::shared_ptr<Scene>> scenes;
    String projectName;
    AutomationManager automationManager;
    int64 videoStartFrame;
    Video video;
    int64 videoSyncOffset;
    std::vector<std::shared_ptr<Track>> tracks;
    std::weak_ptr<Track> audioOutputTrack;

    int64 viewStartSample = 0;
    int64 viewEndSample = 48000 * 30;
    float frameRate=25;
};
