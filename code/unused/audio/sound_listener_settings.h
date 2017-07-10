#pragma once
#include "base/math/vec3.h"

namespace engine
{
using namespace base;

struct SoundListenerSettings
{
	Vec3 position, velocity, forwardDirection, upDirection;
};

}