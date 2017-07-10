#pragma once
#include "base/defines.h"
#include "core/defines.h"
#include "core/resource.h"

namespace engine
{
enum class SoundMode
{
	Mode2D,
	Mode3D
};

enum class SoundLoopMode
{
	Once,
	Repeat
};

struct SoundResource : Resource
{
	u8* data = nullptr;
	u32 dataSize = 0;
	SoundMode mode = SoundMode::Mode3D;
};

}