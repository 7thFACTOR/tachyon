// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/array.h"
#include "base/string.h"
#include "base/dictionary.h"
#include "base/types.h"
#include "base/math/vec3.h"

namespace engine
{
using namespace base;

enum class SoundStatus
{
	Stopped,
	Paused,
	Playing
};
struct AudioSpectatorParameters
{
	Vec3 position;
	Vec3 direction = Vec3::forward();
	Vec3 up = Vec3::up();
};

}