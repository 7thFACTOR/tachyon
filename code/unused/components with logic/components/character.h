#pragma once
#include "base/defines.h"
#include "base/math/vec3.h"
#include "base/math/matrix.h"
#include "render/frustum.h"
#include "game/component.h"
#include "core/resource.h"

namespace engine
{
//! A camera scene object, is the camera through where all scene is viewed
struct CharacterComponent : Component
{
	enum
	{
		TypeId = ComponentType::Character,
		DefaultCount = kMaxComponentCountPerType
	};

	CharacterComponent() {}

	ResourceId mesh = kInvalidResourceId;
	//class CharacterAnimationGraph* animationGraph;
};

}