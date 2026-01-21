#include "Scene.h"
#include <Utils/Uuid.h>

Scene::Scene(int sceneStartSample, int sceneEndSample, String name)
    : id(uuid::generate_uuid_v4()),
      startFrame(0),
      endFrame(0),
      sceneStartSample(sceneStartSample),
      sceneEndSample(sceneEndSample),
      name(name)
{

}

void Scene::split(String nextSceneName, int startShot)
{
    // Empty implementation
}
