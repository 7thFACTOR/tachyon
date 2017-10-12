// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "logic/component.h"

namespace engine
{
class SoundSource;

struct AudioSourceComponent : Component
{
	static const ComponentTypeId typeId = StdComponentTypeId_AudioSource;

	SoundSource* soundSource = nullptr;
};

}