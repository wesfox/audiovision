#pragma once

#include <JuceHeader.h>
#include <Core/Track/Track.h>
#include "Marker.h"

class Scene {
public:
    Scene(int sceneStartSample, int sceneEndSample, const String &name);
    ~Scene() = default;

    // Methods
    void split(String nextSceneName, int startShot);
    void addTrack(std::weak_ptr<Track> track);
    const std::vector<std::weak_ptr<Track>>& getTracks() const;
    const String& getName() const { return name; }

    int64 sceneStartSample;
    int64 sceneEndSample;

private:
    String id;
    int64 startFrame;
    int64 endFrame;
    std::vector<std::weak_ptr<Track>> tracks;
    std::vector<void*> shots;
    //GraphManager graphManager;
    String name;
    std::vector<Marker> markers;
};
