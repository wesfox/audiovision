#pragma once

#include <JuceHeader.h>
#include <Utils/Transport.h>
#include "Scene.h"
#include <Core/Automation/AutomationManager.h>
#include <Core/Video/Video.h>

class Edit {
public:
    Edit();
    ~Edit() = default;

    void addTrack(std::shared_ptr<Track> track) {
        tracks.push_back(std::move(track));
    }

    std::vector<std::shared_ptr<Track>>& getTracks() {
        return tracks;
    }

    std::weak_ptr<Track> getAudioOutputTrack() {
        return audioOutputTrack;
    }

    std::weak_ptr<Track> setAudioOutputTrack(const std::shared_ptr<Track>& audioOutputTrackToSet) {
        return audioOutputTrack = audioOutputTrackToSet;
    }


private:
    Transport transport;
    std::vector<Scene> scenes;
    String projectName;
    AutomationManager automationManager;
    int64 videoStartFrame;
    Video video;
    int64 videoSyncOffset;
    std::vector<std::shared_ptr<Track>> tracks;
    std::weak_ptr<Track> audioOutputTrack;
};
