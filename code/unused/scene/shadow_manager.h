#pragma once
#include "base/array.h"
#include "base/defines.h"
#include "core/defines.h"
#include "base/types.h"
#include "base/class_registry.h"

namespace engine
{
using namespace base;

class SceneRenderer;
class Light;

struct PssmSlice
{
	f32 m_near = 0.0f;
	f32 m_far = 0.0f;
	u32 m_shadowMapSize = 0;
};

//! built-in shadow manager, handles world shadow map from a sun directional light and other local lights
//! PSSM is used
class E_API ShadowManager : public ClassInfo
{
public:
	B_DECLARE_CLASS(ShadowManager);
	ShadowManager();
	virtual ~ShadowManager();

	virtual void create();
	// world shadow maps
	virtual void renderWorldShadowMaps(SceneRenderer* pScene);
	virtual void createWorldShadowMaps(u32 maxMapSize = 1024, u32 mapCount = 3);
	virtual void destroyWorldShadowMaps();
	//! local light shadow maps
	virtual void gatherAffectedSceneObjects(Light* pLight);
	virtual void renderLightShadowMaps(Light* pLight);

protected:
	Light* m_pSun;
	u32 m_maxWorldShadowMapSize;
	Array<PssmSlice> m_pssmSlices;
};

}