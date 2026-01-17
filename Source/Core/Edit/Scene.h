#pragma once

#include <JuceHeader.h>
#include <Core/Track/Track.h>
#include <AudioEngine/Graph/GraphManager.h>
#include "Marker.h"

class Scene {
public:
    Scene();
    ~Scene() = default;

    // Methods
    void split(String nextSceneName, int startShot);

private:
    String id;
    int64 startFrame;
    int64 endFrame;
    int64 sceneStartSample;
    int64 sceneEndSample;
    std::vector<Track*> tracks;
    std::vector<void*> shots;
    GraphManager graphManager;
    String name;
    std::vector<Marker> markers;
};
