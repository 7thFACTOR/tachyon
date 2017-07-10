#pragma once
#include "base/defines.h"
#include "core/defines.h"
#include "base/math/vec3.h"
#include "base/math/matrix.h"
#include "base/math/smoother.h"
#include "game/components/transform.h"
#include "render/frustum.h"
#include "render/scene/camera_node.h"

namespace engine
{
using namespace base;

class InputMap;

struct FreeCameraControllerComponent : Component
{
	enum
	{
		TypeId = ComponentType::FreeCameraController,
		DefaultCount = kMaxComponentCountPerType
	};

	FreeCameraControllerComponent()
		: currentSpeed(0.0f)
		, speed(10.0f)
		, runMultiplier(5.0f)
		, pInputMap(nullptr)
	{}

	f32 currentSpeed;
	f32 speed;
	f32 runMultiplier;
	SmootherVec3 cameraRotationSmoother;
	InputMap* pInputMap;
};

}