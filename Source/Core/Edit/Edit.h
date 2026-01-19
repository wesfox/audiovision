#pragma once

#include <JuceHeader.h>
#include <Utils/Transport.h>
#include "Scene.h"
#include <Core/Automation/AutomationManager.h>
#include <Core/Video/Video.h>
#include <AudioEngine/AudioOutputEngine.h>

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

    std::shared_ptr<juce::AudioProcessorGraph> getAudioGraph() const {
        return audioGraph;
    }

    AudioOutputEngine& getAudioOutputEngine() const {
        return *audioOutputEngine;
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
    std::shared_ptr<juce::AudioProcessorGraph> audioGraph;
    std::unique_ptr<AudioOutputEngine> audioOutputEngine;
};
