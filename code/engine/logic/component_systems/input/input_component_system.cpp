// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include <string.h>
#include "logic/component_systems/input/input_component_system.h"
#include "logic/component_systems/render/render_component_system.h"
#include "core/globals.h"
#include "logic/logic.h"
#include "core/globals.h"
#include "core/application.h"
#include "base/logger.h"
#include "base/profiler.h"
#include "base/qsort.h"
#include "base/variable.h"
#include "base/math/util.h"
#include "logic/component.h"
#include "base/math/conversion.h"
#include "input/input_map.h"
#include "base/array.h"
#include "logic/component_system.h"
#include "logic/types.h"
#include "graphics/graphics.h"
#include "logic/component_pool.h"
#include "input/window.h"
#include "logic/entity.h"
#include "logic/components/render/camera.h"
#include "logic/components/render/transform.h"
#include "logic/components/input/flyby.h"

namespace engine
{
InputComponentSystem::InputComponentSystem()
{
	rotationSmoother.setSampleCount(4);
}

InputComponentSystem::~InputComponentSystem()
{}

void cameraAddYawAngle(CameraComponent* camera, TransformComponent* transform, f32 degrees)
{
	Quat tempQ;
	Vec3 up;
	Matrix rotationMtx = toMatrix(transform->rotation);

	if (camera->correctUpSideDownYaw)
	{
		up = transform->upAxis * B_SGN(rotationMtx.getUpAxis().y);
	}
	else
	{
		up = transform->upAxis;
	}
	
	if (camera->autoUpAxis)
	{
		rotationMtx.getRotationOnly().transform(Vec3(0, 1, 0), up);
		up.normalize();
		transform->upAxis = up;
	}

	toQuat(degrees, up, tempQ);
	transform->rotation = tempQ * transform->rotation;
	transform->rotation.normalize();
	transform->dirty = true;
}

void cameraAddPitchAngle(CameraComponent* camera, TransformComponent* transform, f32 degrees)
{
	Quat tempQ;

	// make pitch quat
	toQuat(degrees, 1.0f, 0.0f, 0.0f, tempQ);
	// add before
	transform->rotation *= tempQ;
	transform->rotation.normalize();
	transform->dirty = true;
}

void xformMove(TransformComponent* xform, const Vec3& delta)
{
	if (delta.isEquivalent(Vec3(), 0.0001f))
	{
		return;
	}

	Matrix m;
	Vec3 deltaLocal;

	toMatrix(xform->rotation, m);
	m.transform(delta, deltaLocal);
	xform->translation += BigVec3(deltaLocal.x, deltaLocal.y, deltaLocal.z);
	xform->dirty = true;
}

void InputComponentSystem::update(f32 deltaTime)
{
	if (!getGraphics().getWindow()->isFocused())
	{
		return;
	}

	updateFlyBy(deltaTime);
}

void InputComponentSystem::updateFlyBy(f32 deltaTime)
{
	auto mousePos = Mouse::getPosition(*getGraphics().getWindow());
	Vec2 windowCenter = {
		getGraphics().getWindow()->getSize().x / 2.0f,
		getGraphics().getWindow()->getSize().y / 2.0f };

	if (!mouseWasCentered)
	{
		Mouse::setPosition({ (i32)windowCenter.x, (i32)windowCenter.y }, *getGraphics().getWindow());
		mouseWasCentered = false;
	}

	Vec2 currentMousePosition = Vec2(mousePos.x, mousePos.y);
	Vec2 mouseDelta = currentMousePosition - windowCenter;
	Vec2 rotation = { mouseDelta.x / getGraphics().getWindow()->getSize().x, mouseDelta.y / getGraphics().getWindow()->getSize().y };
	f32 mouseSpeed = getVariableRegistry().getVariableValue<f32>("inp_mouse_speed");
	bool validEvent = false;

	if (!rotation.isEquivalent(Vec2()))
	{
		validEvent = true;
		rotationSmoother.addValue(rotation);
		rotation = rotationSmoother.compute();
	}

	Vec3 moveDelta;

	if (getLogic().inputMap.isAction("moveForward"))
	{
		moveDelta.z = 1.0f;
	}
	else if (getLogic().inputMap.isAction("moveBackward"))
	{
		moveDelta.z = -1.0f;
	}

	if (getLogic().inputMap.isAction("moveLeft"))
	{
		moveDelta.x = -1.0f;
	}
	else if (getLogic().inputMap.isAction("moveRight"))
	{
		moveDelta.x = 1.0f;
	}

	moveDelta.normalize();

	// update FlybyComponent
	auto flybyPool = getLogic().getComponentPool<FlybyComponent>();

	for (size_t i = 0; i < flybyPool->getComponentCount(); i++)
	{
		auto flyby = flybyPool->getComponentAs<FlybyComponent>(i);

		if (!flyby->used)
		{
			continue;
		}

		auto xform = flyby->entity->getComponent<TransformComponent>();
		auto camera = flyby->entity->getComponent<CameraComponent>();

		if (moveDelta.getLength() <= 0.0f)
		{
			flyby->currentSpeed = flyby->speed;
		}
		else
		{
			flyby->currentSpeed += flyby->currentSpeed * flyby->acceleration * deltaTime;
		}

		Vec3 advanceBy = moveDelta * flyby->currentSpeed * deltaTime;
		
		if (getLogic().inputMap.isAction("run"))
		{
			advanceBy *= flyby->speedWarpScale;
		}

		xformMove(xform, advanceBy);

		if (validEvent)
		{
			cameraAddPitchAngle(camera, xform, rotation.y * mouseSpeed);
			cameraAddYawAngle(camera, xform, rotation.x * mouseSpeed);
		}

		xform->dirty = true;
	}

	Mouse::setPosition({ (i32)windowCenter.x, (i32)windowCenter.y }, *getGraphics().getWindow());
}

}