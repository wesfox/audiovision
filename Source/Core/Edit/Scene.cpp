#include "Scene.h"
#include <Utils/Uuid.h>

Scene::Scene(int sceneStartSample, int sceneEndSample, const String& name)
    : id(uuid::generate_uuid_v4()),
      startFrame(0),
      endFrame(0),
      sceneStartSample(sceneStartSample),
      sceneEndSample(sceneEndSample),
      name(std::move(name))
{

}

void Scene::split(String nextSceneName, int startShot)
{
    // Empty implementation
}

void Scene::addTrack(std::weak_ptr<Track> track)
{
    tracks.push_back(std::move(track));
}

const std::vector<std::weak_ptr<Track>>& Scene::getTracks() const
{
    return tracks;
}
