// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "core/defines.h"
#include "core/types.h"
#include "base/math/vec3.h"

namespace engine
{
using namespace base;

class E_API Audio
{
public:
	bool initialize() { return true; }
	void shutdown() {}
	void setGlobalVolume(f32 volume);
	f32 getGlobalVolume();
	void setListenerPosition(const Vec3& position);
	Vec3 getListenerPosition();
	void setListenerDirection(const Vec3& direction);
	Vec3 getListenerDirection();
	void setUpVector(const Vec3& up);
	Vec3 getUpVector();
};
	
}