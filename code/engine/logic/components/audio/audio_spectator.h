#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "logic/component.h"
#include "audio/types.h"

namespace engine
{
struct AudioSpectatorComponent : Component
{
	static const ComponentTypeId typeId = StdComponentTypeId_AudioSpectator;

	AudioSpectatorParameters spectatorParams;
};

}