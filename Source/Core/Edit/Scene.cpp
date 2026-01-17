#include "Scene.h"
#include <Utils/Uuid.h>

Scene::Scene()
    : id(uuid::generate_uuid_v4()),
      startFrame(0),
      endFrame(0),
      sceneStartSample(0),
      sceneEndSample(0),
      graphManager(),
      name("")
{
}

void Scene::split(String nextSceneName, int startShot)
{
    // Empty implementation
}
