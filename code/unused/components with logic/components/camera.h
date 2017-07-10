#pragma once
#include "base/defines.h"
#include "core/defines.h"
#include "base/math/vec3.h"
#include "base/math/matrix.h"
#include "game/components/transform.h"
#include "render/frustum.h"
#include "render/scene/camera_node.h"
#include "game/types.h"

namespace engine
{
struct CameraComponent : Component
{
	enum
	{
		Type = ComponentType::Camera,
		DefaultCount = kMaxComponentCountPerType
	};

	CameraComponent() {}

	SceneNode* pNode = nullptr;
	CameraData data;
};

}