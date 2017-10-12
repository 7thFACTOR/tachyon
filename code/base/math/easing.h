// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"

namespace base
{
// Copyright (C) Robert Penner's Easing Functions
// http://www.robertpenner.com/easing/

// Function parameter meaning:
// t - is the current time(or position) of the tween.This can be seconds or frames, steps, seconds, ms, whatever – as long as the unit is the same as is used for the total time[3].
// b - is the beginning value of the property.
// c - is the change between the beginning and destination value of the property.
// d - is the total time of the tween.

typedef f32 (*EaseFunction)(f32 t, f32 b, f32 c, f32 d);

struct B_API Easing
{
static f32 inLinear(f32 t, f32 b, f32 c, f32 d);
static f32 outLinear(f32 t, f32 b, f32 c, f32 d);
static f32 inOutLinear(f32 t, f32 b, f32 c, f32 d);

static f32 inBack(f32 t, f32 b, f32 c, f32 d);
static f32 outBack(f32 t, f32 b, f32 c, f32 d);
static f32 inOutBack(f32 t, f32 b, f32 c, f32 d);

static f32 inBounce(f32 t, f32 b, f32 c, f32 d);
static f32 outBounce(f32 t, f32 b, f32 c, f32 d);
static f32 inOutBounce(f32 t, f32 b, f32 c, f32 d);

static f32 inCirc(f32 t, f32 b, f32 c, f32 d);
static f32 outCirc(f32 t, f32 b, f32 c, f32 d);
static f32 inOutCirc(f32 t, f32 b, f32 c, f32 d);

static f32 inCubic(f32 t, f32 b, f32 c, f32 d);
static f32 outCubic(f32 t, f32 b, f32 c, f32 d);
static f32 inOutCubic(f32 t, f32 b, f32 c, f32 d);

static f32 inElastic(f32 t, f32 b, f32 c, f32 d);
static f32 outElastic(f32 t, f32 b, f32 c, f32 d);
static f32 inOutElastic(f32 t, f32 b, f32 c, f32 d);

static f32 inExpo(f32 t, f32 b, f32 c, f32 d);
static f32 outExpo(f32 t, f32 b, f32 c, f32 d);
static f32 inOutExpo(f32 t, f32 b, f32 c, f32 d);

static f32 inQuad(f32 t, f32 b, f32 c, f32 d);
static f32 outQuad(f32 t, f32 b, f32 c, f32 d);
static f32 inOutQuad(f32 t, f32 b, f32 c, f32 d);

static f32 inQuart(f32 t, f32 b, f32 c, f32 d);
static f32 outQuart(f32 t, f32 b, f32 c, f32 d);
static f32 inOutQuart(f32 t, f32 b, f32 c, f32 d);

static f32 inQuint(f32 t, f32 b, f32 c, f32 d);
static f32 outQuint(f32 t, f32 b, f32 c, f32 d);
static f32 inOutQuint(f32 t, f32 b, f32 c, f32 d);

static f32 inSine(f32 t, f32 b, f32 c, f32 d);
static f32 outSine(f32 t, f32 b, f32 c, f32 d);
static f32 inOutSine(f32 t, f32 b, f32 c, f32 d);
};

}