#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/map.h"
#include "base/defines.h"
#include "base/types.h"
#include "game/component.h"
#include "audio/audio_manager.h"

namespace engine
{
struct SoundListenerComponent : Component
{
	enum
	{
		TypeId = ComponentType::SoundListener,
		DefaultCount = kMaxComponentCountPerType
	};

	SoundListener listener;
};

}