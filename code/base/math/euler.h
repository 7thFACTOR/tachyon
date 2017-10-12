// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/math/vec3.h"

namespace base
{
class B_API Euler
{
public:
	enum class RotationOrder
	{
		XYZ,
		YZX,
		ZXY,
		XZY,
		ZYX,
		YXZ
	};
	
	Euler()
		: x(0.0f)
		, y(0.0f)
		, z(0.0f)
	{}

	Euler(f32 newX, f32 newY, f32 newZ)
		: x(newX)
		, y(newY)
		, z(newZ)
	{}

	Euler(const Vec3& angles)
	{
		set(angles);
	}

	void set(const Vec3& angles)
	{
		x = angles.x;
		y = angles.y;
		z = angles.z;
	}

	void set(f32 newX, f32 newY, f32 newZ)
	{
		x = newX;
		y = newY;
		z = newZ;
	}

	f32 x, y, z;
};

}