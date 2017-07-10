#include "render/scene/scene.h"
#include "render/scene/scene_node.h"
#include "base/logger.h"
#include "core/core.h"

namespace engine
{
Scene::Scene()
{
	m_pRoot = new SceneNode();
	m_pRoot->m_pScene = this;
}

Scene::~Scene()
{
	delete m_pRoot;
}

void Scene::update(f32 deltaTime)
{
	m_pRoot->update(deltaTime);
}

}