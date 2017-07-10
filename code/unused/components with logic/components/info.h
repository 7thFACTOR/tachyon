#pragma once
#include "base/defines.h"
#include "game/component.h"

namespace engine
{
//! A camera scene object, is the camera through where all scene is viewed
struct InfoComponent : Component
{
	enum
	{
		TypeId = ComponentType::Info,
		DefaultCount = kMaxComponentCountPerType
	};

	InfoComponent()
	{}

	String name;
	Map<String, String> tags;
};
}