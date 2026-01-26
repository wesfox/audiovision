#pragma once

#include <JuceHeader.h>
#include <Core/Track/Track.h>
#include "Marker.h"

/// Scene grouping tracks and timeline region.
class Scene {
public:
    /// Create a scene spanning a timeline range.
    /// @param sceneStartSample start sample in the timeline
    /// @param sceneEndSample end sample in the timeline
    /// @param name scene display name
    Scene(int sceneStartSample, int sceneEndSample, const String &name);
    ~Scene() = default;

    /// Split this scene into two scenes.
    /// @param nextSceneName name for the next scene
    /// @param startShot first shot index in the new scene
    void split(String nextSceneName, int startShot);

    /// Attach a track to this scene.
    /// @param track track to add
    void addTrack(std::weak_ptr<Track> track);

    /// Access tracks attached to this scene.
    const std::vector<std::weak_ptr<Track>>& getTracks() const;

    /// Scene display name.
    const String& getName() const { return name; }

    /// Scene start sample in the timeline.
    int64 sceneStartSample;
    /// Scene end sample in the timeline.
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
