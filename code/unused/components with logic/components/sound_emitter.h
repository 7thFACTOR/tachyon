#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/map.h"
#include "base/defines.h"
#include "base/types.h"
#include "game/component.h"

namespace engine
{
class SoundSource;

struct SoundEmitterComponent : Component
{
	enum
	{
		TypeId = ComponentType::SoundEmitter,
		DefaultCount = kMaxComponentCountPerType
	};

	SoundSource* soundSource;
};

}