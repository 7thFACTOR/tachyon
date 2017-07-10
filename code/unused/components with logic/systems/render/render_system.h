#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/map.h"
#include "base/defines.h"
#include "core/defines.h"
#include "base/types.h"
#include "base/observer.h"
#include "base/math/ray.h"
#include "game/component.h"
#include "game/game_system.h"
#include "render/renderer.h"
#include "game/components/first_person_view.h"

namespace engine
{
using namespace base;

class ShadowManager;
struct TransformComponent;
struct CameraComponent;
struct MeshComponent;
class PostProcessManager;
class SceneRenderer;

class E_API RenderSystem : public GameSystem
{
public:
	enum { Type = GameSystemType::Render };

	B_DECLARE_DERIVED_CLASS(RenderSystem, GameSystem);
	RenderSystem();
	virtual ~RenderSystem();
	virtual void update();
	void onEntityCreated(EntityId aId);
	void onEntityRemoved(EntityId aId);
	virtual void onEntityParentChange(EntityId entity, EntityId oldParent, EntityId newParent);
	virtual void onComponentAdded(EntityId aId, Component* pComp);
	virtual void onComponentRemoved(EntityId aId, Component* pComp);

	struct E_API Transform
	{
		static void update(TransformComponent* pXform);
		static void move(TransformComponent* pXform, const Vec3& delta);
	};

	struct E_API Mesh
	{
		static void update(MeshComponent* pMesh);
	};

	struct E_API FirstPersonView
	{
	static bool createPhysicsCapsule(FirstPersonViewComponent* pFpView, f32 aCapsuleRadius = 0.5f, f32 aCapsuleHeight = 2.0f);
	static void destroyPhysicsCapsule(FirstPersonViewComponent* pFpView);
	static void reset(FirstPersonViewComponent* pFpView);
	static void update(FirstPersonViewComponent* pFpView, CameraComponent* pCamera, TransformComponent* pTransform);
	static void startEffect(FirstPersonViewComponent* pFpView, CameraEffect aEffect, f32 aParam1 = 0.0f, f32 aParam2 = 0.0f, f32 aParam3 = 0.0f);
	static void stopEffect(FirstPersonViewComponent* pFpView, CameraEffect aEffect);
	static bool isEffectRunning(FirstPersonViewComponent* pFpView, CameraEffect aEffect);
	static void move(FirstPersonViewComponent* pFpView, const Vec3& rDelta, f32 aMinDistance = 0.0001f, f32 aAutoStepSharpness = 0.5f);
	static void jump(FirstPersonViewComponent* pFpView);
	static void crouch(FirstPersonViewComponent* pFpView);
	static void standUp(FirstPersonViewComponent* pFpView);
	static bool canStandUp(FirstPersonViewComponent* pFpView);
	static void lean(FirstPersonViewComponent* pFpView, LeaningDirection aDir);
	static bool isMoving(FirstPersonViewComponent* pFpView);
	static bool isRunning(FirstPersonViewComponent* pFpView);
	};

protected:
	SceneRenderer* m_pRenderer;
	Array<TransformComponent*> m_transformComponents;
	Array<CameraComponent*> m_cameraComponents;
	Array<MeshComponent*> m_meshComponents;
};
}