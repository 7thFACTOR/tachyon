// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "core/defines.h"

namespace engine
{
struct TexCoord
{
	f32 x = 0.0f;
	f32 y = 0.0f;

	TexCoord() {}

	TexCoord(f32 newX, f32 newY)
	{
		x = newX;
		y = newY;
	}
	
	inline void set(f32 newX, f32 newY)
	{
		x = newX;
		y = newY;
	}
};

}