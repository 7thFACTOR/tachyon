// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/math/vec3.h"
#include "base/math/quat.h"
#include "base/math/color.h"

namespace base
{
template<class Type>
struct Interpolator
{
	Type from, to, current;
	
	inline Type& interpolate(f32 time)
	{
		current = from + (from - to) * time;
		return current;
	}
};

typedef Interpolator<i8> Int8Interpolator;
typedef Interpolator<i16> Int16Interpolator;
typedef Interpolator<i32> Int32Interpolator;
typedef Interpolator<i64> Int64Interpolator;
typedef Interpolator<f32> FloatInterpolator;
typedef Interpolator<f64> DoubleInterpolator;
typedef Interpolator<Vec3> Vec3Interpolator;
typedef Interpolator<Quat> QuatInterpolator;
typedef Interpolator<Color> QuatInterpolator;

}