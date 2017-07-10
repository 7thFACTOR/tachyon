#pragma once
#include "base/defines.h"
#include "base/array.h"
#include "base/string.h"
#include "base/dictionary.h"
#include "base/types.h"
#include "base/math/vec3.h"
#include "core/defines.h"
#include "base/class_registry.h"

namespace engine
{
using namespace base;

struct AudioSpectatorParameters
{
	Vec3 position;
	Vec3 velocity;
	Vec3 forwardDirection = Vec3(0, 0, 1);
	Vec3 upDirection = Vec3(0, 1, 0);
};

struct AudioSettings
{
	i32 frequency = 44100;
	i32 soundBufferSize = 0;
	Array<String> drivers;
	Array<String> outputs;
	i32 currentDriver = 0;
	i32 currentOutput = 0;
};

}