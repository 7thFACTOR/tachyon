#include "render/scene/shadow_manager.h"
#include "core/core.h"

namespace engine
{
B_REGISTER_CLASS(ShadowManager);

ShadowManager::ShadowManager()
{}

ShadowManager::~ShadowManager()
{}

void ShadowManager::create()
{}

void ShadowManager::renderWorldShadowMaps(SceneRenderer* pScene)
{}

void ShadowManager::createWorldShadowMaps(u32 maxMapSize, u32 mapCount)
{}

void ShadowManager::destroyWorldShadowMaps()
{}

void ShadowManager::gatherAffectedSceneObjects(Light* pLight)
{}

void ShadowManager::renderLightShadowMaps(Light* pLight)
{}

}