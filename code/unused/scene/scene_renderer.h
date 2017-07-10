#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/map.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/class_registry.h"
#include "base/observer.h"
#include "base/math/ray.h"
#include "game/component.h"

namespace engine
{
using namespace base;
class ShadowManager;
struct TransformComponent;
struct RenderPacket;
class PostProcessManager;
class Scene;
class SceneNode;

class E_API SceneRenderer
{
public:
	SceneRenderer();
	virtual ~SceneRenderer();

	virtual void update(Scene* pScene);
	virtual void render(Scene* pScene);
	ShadowManager* shadowManager() const;

protected:
	virtual void renderNodes(u32 flags);
	void gatherNodes();
	void gatherNodesRecursive(SceneNode* pNode);

	Scene* m_pScene;
	Array<SceneNode*> m_nodes;
	ShadowManager* m_pShadowManager;
	PostProcessManager* m_pPostProcessManager;
};
}