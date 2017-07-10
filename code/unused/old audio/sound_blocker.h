#pragma once
#include "base/array.h"
#include "base/defines.h"
#include "base/types.h"
#include "core/defines.h"
#include "base/math/vec3.h"

namespace engine
{
using namespace base;

struct SoundBlockerPolygon
{
	Array<Vec3> vertices;
	bool doubleSide = false;
	//! sound pass through factor, 0 = sound travels all the way
	//! through this polygon, 1 = sound is blocked
	f32 directOcclusion = 1.0f;
	//! reverb sound pass through factor, for 0 = sound travels all the way
	//! through this polygon, 1 = sound is blocked
	f32 reverbOcclusion = 1.0f;
	//! index of the polygon in the blocker object list
	u32 index = 0;
};

struct SoundBlocker
{
	Array<SoundBlockerPolygon> polygons;
};

}