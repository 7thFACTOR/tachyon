// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/dictionary.h"
#include "base/defines.h"
#include "core/defines.h"
#include "base/types.h"
#include "base/observer.h"
#include "base/math/ray.h"
#include "logic/component.h"
#include "logic/component_system.h"
#include "graphics/graphics.h"
#include "base/observer.h"

namespace engine
{
using namespace base;

struct TransformComponent;
struct CameraComponent;
struct MeshComponent;
class ShapeRenderer;

struct RenderObserver
{
	virtual void onBeforeWorldRender() {}
	virtual void onAfterWorldRender() {}
};

class RenderComponentSystem : public ComponentSystem, public Observable<RenderObserver>
{
public:
	RenderComponentSystem();
	virtual ~RenderComponentSystem();
	ComponentSystemId getId() const override { return StdComponentSystemId_Render; }
	void update(f32 deltaTime) override;

protected:
	void updateCamera(CameraComponent& camera);
	void updateLocalTransform(TransformComponent& xform);
	void updateWorldTransform(TransformComponent& xform);
	GpuProgram* setupMaterial(struct MeshRendererComponent* meshRenderer, u32 matIndex);

	Entity* currentCamera = nullptr;
	ShapeRenderer* shapeRenderer = nullptr;
};

}