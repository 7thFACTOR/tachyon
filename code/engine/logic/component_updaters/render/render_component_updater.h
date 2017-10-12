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
#include "logic/component_updater.h"
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

class RenderComponentUpdater : public ComponentUpdater, public Observable<RenderObserver>
{
public:
	RenderComponentUpdater();
	virtual ~RenderComponentUpdater();
	ComponentUpdaterId getId() const override { return StdComponentUpdaterId_Render; }
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