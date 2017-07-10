#pragma once
//*****************************************************************************
// Nytro Game Engine
//
// (C) Copyright 7thFACTOR Entertainment
// All rights reserved
//*****************************************************************************
#include "system/string.h"
#include "system/array.h"
#include "system/map.h"
#include "system/defines.h"
#include "system/types.h"
#include "system/rtti.h"
#include "system/observer.h"
#include "math/ray.h"
#include "system/rtti.h"
#include "game/component.h"

namespace nytro
{
class ShadowManager;
struct TransformComponent;
class RenderPacket;
class RenderContext;
class PostProcessManager;

class N_API SceneRenderer2D: public Rtti
{
public:
	N_RUNTIME_CLASS;
	SceneRenderer2D();
	virtual ~SceneRenderer2D();

	virtual void createDefaultManagers();
	virtual void onUpdate();
	//! called each frame to render the sorted render packets, the actual scene drawing goes here
	virtual void render(RenderContext& rCtx, u32 aRenderMode = 0);
	//! \return the current shadow manager
	ShadowManager* shadowManager() const;

protected:
	//! internal, used for rendering the render packets
	virtual void renderPackets(RenderContext& rCtx, u32 aFlags);
	virtual void gatherRenderPackets();

	//! the render packets to be sorted and rendered
	Array<RenderPacket*> m_renderPackets;
	//! the post manager
	PostProcessManager* m_pPostProcessManager;
};
}