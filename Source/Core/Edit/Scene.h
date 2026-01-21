#pragma once

#include <JuceHeader.h>
#include <Core/Track/Track.h>
#include "Marker.h"

class Scene {
public:
    Scene(int sceneStartSample, int sceneEndSample, String name);
    ~Scene() = default;

    // Methods
    void split(String nextSceneName, int startShot);

    int64 sceneStartSample;
    int64 sceneEndSample;

private:
    String id;
    int64 startFrame;
    int64 endFrame;
    std::vector<Track*> tracks;
    std::vector<void*> shots;
    //GraphManager graphManager;
    String name;
    std::vector<Marker> markers;
};
