// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/types.h"

namespace base
{
	const double twoPi = 6.283185307f;
	const double pi = 3.141592654f;
	const double halfPi = 1.570796327f;
	const double epsilon = 0.001f;
	const double epsilon2 = 1.0e-6;
	const double piOver180 = pi / 180.0f;
	const double deg180OverPi = 180.0f / pi;
	const double piMul180 = pi * 180.0f;
	const f32 meterUnit = 1.0f;
	const f32 feetUnit = meterUnit * 0.3048f;
	const f32 oneKm = meterUnit * 1000.0f;
	const f32 oneMile = oneKm * 1.609344f;
	const f32 oneNauticalMile = oneKm * 1.852f;

#ifndef M_PI
	const double M_PI = pi;
#endif

#ifndef PI
	const double PI = pi;
#endif

#define B_DEGREES_TO_RADIANS(phi) ((phi) * piOver180)

#define B_RADIANS_TO_DEGREES(phi) ((phi) * deg180OverPi)

#define B_COSINUS_TO_DEGREES(phi) (acosf( phi ) * deg180OverPi)

#define B_LERP(a1, a2, t) ((a1) + ((a2) - (a1)) * (t))

#define B_ABS(a) (((a) < 0) ? (-(a)) : (a))

#define B_SGN(a) (((a) < 0) ? (-1) : (1))

#define B_KMPH_TO_MPS(km) ((f32)km * 1000.0f / 3600.0f)
}