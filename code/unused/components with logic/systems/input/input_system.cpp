#include <string.h>
#include "game/systems/input/input_system.h"
#include "game/systems/render/render_system.h"
#include "core/core.h"
#include "game/game_manager.h"
#include "core/core.h"
#include "base/assert.h"
#include "base/logger.h"
#include "base/profiler.h"
#include "base/qsort.h"
#include "base/variable.h"
#include "game/component.h"
#include "game/game_manager.h"
#include "base/math/conversion.h"
#include "input/mouse.h"
#include "input/input_manager.h"
#include "input/input_map.h"
#include "base/array.h"
#include "game/component_type_manager.h"
#include "game/types.h"

#include "game/components/camera.h"
#include "game/components/transform.h"
#include "game/components/first_person_view.h"
#include "game/components/free_camera_controller.h"

namespace engine
{
B_REGISTER_CLASS(InputSystem);

InputSystem::InputSystem()
{
	E_REGISTER_COMPONENT_MANAGER(FreeCameraControllerComponent);
}

InputSystem::~InputSystem()
{
	E_UNREGISTER_COMPONENT_MANAGER(FreeCameraControllerComponent);
}

void InputSystem::update()
{
	for (size_t i = 0, iCount = m_freeCamComponents.size(); i < iCount; ++i)
	{
		update(m_freeCamComponents[i]);
	}
}

void InputSystem::onComponentAdded(EntityId entity, Component* pComp)
{
	switch (pComp->typeId)
	{
	case ComponentType::FreeCameraController:
		{
			auto comp = (FreeCameraControllerComponent*)pComp;
			m_freeCamComponents.appendUnique(comp);
			break;
		}
	}
}

void InputSystem::onComponentRemoved(EntityId aId, Component* pComp)
{
	switch (pComp->typeId)
	{
	case ComponentType::FreeCameraController:
		{
			m_freeCamComponents.erase((FreeCameraControllerComponent*)pComp);
			break;
		}
	}
}

void InputSystem::update(FreeCameraControllerComponent* pComp)
{
	if (engineMode() == EngineModes::Editing)
	{
		if (!input().mouse()->isButtonDown(MouseButtons::Right))
		{
			return;
		}
	}

	if (!pComp->pInputMap)
	{
		return;
	}

	// camera movement speed in units (meters) per second
	const f32 cCameraMoveSpeed = pComp->speed * kMeterUnit;
	const f32 cCameraRunSpeedMul = pComp->runMultiplier;
	bool bIsMoving = false;
	Vec3 actorMoveDelta;
	f32 runningMul = 1;

	if (pComp->pInputMap->isAction("run"))
	{
		runningMul = cCameraRunSpeedMul;
	}

	pComp->currentSpeed = runningMul * cCameraMoveSpeed;

	if (pComp->pInputMap->isAction("moveForward"))
	{
		actorMoveDelta += Vec3(0, 0, 1);
		bIsMoving = true;
	}

	if (pComp->pInputMap->isAction("moveBackward"))
	{
		bIsMoving = true;
		actorMoveDelta += Vec3(0, 0, -1);
	}

	if (pComp->pInputMap->isAction("moveLeft"))
	{
		bIsMoving = true;
		actorMoveDelta += Vec3(-1, 0, 0);
	}

	if (pComp->pInputMap->isAction("moveRight"))
	{
		bIsMoving = true;
		actorMoveDelta += Vec3(1, 0, 0);
	}

	CameraComponent* pCamera = game().component<CameraComponent>(pComp->entity);
	TransformComponent* pTransform = game().component<TransformComponent>(pComp->entity);
	auto renderSys = game().gameSystem<RenderSystem>();

	pComp->cameraRotationSmoother.addValue(
		Vec3(-input().mouse()->delta().x / 10.0f, -input().mouse()->delta().y / 10.0f, 0));

	Vec3 rot = pComp->cameraRotationSmoother.compute();

	pCamera->addYawAngle(rot.x);
	pCamera->addPitchAngle(rot.y);
	//TODO:	pTransform->move(pTransform, actorMoveDelta * pComp->currentSpeed * gameDeltaTime());
	pTransform->bDirty = true;
	pCamera->bDirty = true;
}

}