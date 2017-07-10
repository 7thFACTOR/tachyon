#include "base/math/easing.h"
#include "base/math/util.h"
#include <math.h>

namespace base
{
f32 Easing::inLinear(f32 t, f32 b , f32 c, f32 d)
{
	return c * t / d + b;
}

f32 Easing::outLinear(f32 t, f32 b , f32 c, f32 d)
{
	return c * t / d + b;
}

f32 Easing::inOutLinear(f32 t, f32 b , f32 c, f32 d)
{
	return c * t / d + b;
}

//////////////////////////////////////////////////////////////////////////

f32 Easing::inBack(f32 t, f32 b, f32 c, f32 d)
{
	f32 s = 1.70158f;
	f32 postFix = t /= d;
	return c * (postFix) * t * ((s + 1) * t - s) + b;
}

f32 Easing::outBack(f32 t, f32 b, f32 c, f32 d)
{
	f32 s = 1.70158f;
	return c * ((t = t / d - 1) * t * ((s + 1) * t + s) + 1) + b;
}

f32 Easing::inOutBack(f32 t, f32 b, f32 c, f32 d)
{
	f32 s = 1.70158f;

	if ((t /= d / 2) < 1)
		return c / 2 * (t * t * (((s *= (1.525f)) + 1) * t - s)) + b;

	f32 postFix = t -= 2;
	return c / 2 * ((postFix) * t * (((s *= (1.525f)) + 1) * t + s) + 2) + b;
}

//////////////////////////////////////////////////////////////////////////

f32 Easing::inBounce(f32 t, f32 b, f32 c, f32 d)
{
	return c - outBounce(d - t, 0, c, d) + b;
}

f32 Easing::outBounce(f32 t, f32 b, f32 c, f32 d)
{
	if ((t /= d) < (1 / 2.75f))
	{
		return c * (7.5625f * t * t) + b;
	}
	else if (t < (2 / 2.75f))
	{
		f32 postFix = t -= (1.5f / 2.75f);
		return c * (7.5625f * (postFix) * t + .75f) + b;
	}
	else if (t < (2.5 / 2.75))
	{
		f32 postFix = t -= (2.25f / 2.75f);
		return c * (7.5625f * (postFix) * t + .9375f) + b;
	}
	else
	{
		f32 postFix = t -= (2.625f / 2.75f);
		return c * (7.5625f * (postFix) * t + .984375f) + b;
	}
}

f32 Easing::inOutBounce(f32 t, f32 b, f32 c, f32 d)
{
	if (t < d / 2)
		return inBounce(t * 2, 0, c, d) * .5f + b;

	return outBounce(t * 2 - d, 0, c, d) * .5f + c * .5f + b;
}

//////////////////////////////////////////////////////////////////////////

f32 Easing::inCirc(f32 t, f32 b, f32 c, f32 d)
{
	return -c * (sqrtf(1 - (t /= d) * t) - 1) + b;
}

f32 Easing::outCirc(f32 t, f32 b, f32 c, f32 d)
{
	return c * sqrtf(1 - (t = t / d - 1) * t) + b;
}

f32 Easing::inOutCirc(f32 t, f32 b, f32 c, f32 d)
{
	if ((t /= d / 2) < 1)
		return -c / 2 * (sqrtf(1 - t * t) - 1) + b;

	return c / 2 * (sqrtf(1 - t * (t -= 2)) + 1) + b;
}

//////////////////////////////////////////////////////////////////////////

f32 Easing::inCubic(f32 t, f32 b, f32 c, f32 d)
{
	return c * (t /= d) * t * t + b;
}

f32 Easing::outCubic(f32 t, f32 b, f32 c, f32 d)
{
	return c * ((t = t / d - 1) * t * t + 1) + b;
}

f32 Easing::inOutCubic(f32 t, f32 b, f32 c, f32 d)
{
	if ((t /= d / 2) < 1.0f)
		return c / 2 * t * t * t + b;

	return c / 2 * ((t -= 2) * t * t + 2) + b;
}

//////////////////////////////////////////////////////////////////////////

f32 Easing::inElastic(f32 t, f32 b, f32 c, f32 d)
{
	if (t <= 0.00001f)
		return b;

	if ((t /= d) >= 1.0f)
		return b + c;

	f32 p = d * .3f;
	f32 a = c;
	f32 s = p / 4;
	f32 postFix = a * powf(2, 10 * (t -= 1));

	return -(postFix * sinf((t * d - s) * (2 * pi) / p )) + b;
}

f32 Easing::outElastic(f32 t, f32 b, f32 c, f32 d)
{
	if (fabs(t) <= 0.00001f)
		return b;

	if ((t /= d) >= 1.0f)
		return b + c;

	f32 p = d * .3f;
	f32 a = c;
	f32 s = p / 4;

	return (a * powf(2, -10 * t) * sinf( (t * d - s) * (2 * pi) / p ) + c + b);
}

f32 Easing::inOutElastic(f32 t, f32 b, f32 c, f32 d)
{
	if (t == 0)
		return b;

	if ((t /= d / 2) == 2)
		return b + c;

	f32 p = d * (.3f * 1.5f);
	f32 a = c;
	f32 s = p / 4;

	if (t < 1)
	{
		f32 postFix = a * powf(2, 10 * (t -= 1));
		return -.5f * (postFix * sinf((t * d - s) * (2 * pi) / p)) + b;
	}

	f32 postFix =  a * powf(2, -10 * (t -= 1));
	return postFix * sinf((t * d - s) * (2 * pi) / p) * .5f + c + b;
}

//////////////////////////////////////////////////////////////////////////

f32 Easing::inExpo(f32 t, f32 b, f32 c, f32 d)
{
	return (t == 0) ? b : c * powf(2, 10 * (t / d - 1)) + b;
}

f32 Easing::outExpo(f32 t, f32 b, f32 c, f32 d)
{
	return (t == d) ? b + c : c * (-powf(2, -10 * t / d) + 1) + b;
}

f32 Easing::inOutExpo(f32 t, f32 b, f32 c, f32 d)
{
	if (t == 0)
		return b;

	if (t == d)
		return b + c;

	if ((t /= d / 2) < 1)
		return c / 2 * powf(2, 10 * (t - 1)) + b;

	return c / 2 * (-powf(2, -10 * --t) + 2) + b;
}

//////////////////////////////////////////////////////////////////////////

f32 Easing::inQuad(f32 t, f32 b, f32 c, f32 d)
{
	return c * (t /= d) * t + b;
}

f32 Easing::outQuad(f32 t, f32 b, f32 c, f32 d)
{
	return -c * (t /= d) * (t - 2) + b;
}

f32 Easing::inOutQuad(f32 t, f32 b, f32 c, f32 d)
{
	if ((t /= d / 2) < 1)
		return ((c / 2) * (t * t)) + b;

	return -c / 2 * (((t - 2) * (--t)) - 1) + b;
}

//////////////////////////////////////////////////////////////////////////

f32 Easing::inQuart(f32 t, f32 b, f32 c, f32 d)
{
	return c * (t /= d) * t * t * t + b;
}

f32 Easing::outQuart(f32 t, f32 b, f32 c, f32 d)
{
	return -c * ((t = t / d - 1) * t * t * t - 1) + b;
}

f32 Easing::inOutQuart(f32 t, f32 b, f32 c, f32 d)
{
	if ((t /= d / 2) < 1)
		return c / 2 * t * t * t * t + b;

	return -c / 2 * ((t -= 2) * t * t * t - 2) + b;
}

//////////////////////////////////////////////////////////////////////////

f32 Easing::inQuint(f32 t, f32 b, f32 c, f32 d)
{
	return c * (t /= d) * t * t * t * t + b;
}

f32 Easing::outQuint(f32 t, f32 b, f32 c, f32 d)
{
	return c * ((t = t / d - 1) * t * t * t * t + 1) + b;
}

f32 Easing::inOutQuint(f32 t, f32 b, f32 c, f32 d)
{
	if ((t /= d / 2) < 1)
		return c / 2 * t * t * t * t * t + b;

	return c / 2 * ((t -= 2) * t * t * t * t + 2) + b;
}

//////////////////////////////////////////////////////////////////////////

f32 Easing::inSine(f32 t, f32 b, f32 c, f32 d)
{
	return -c * cosf(t / d * (pi / 2)) + c + b;
}

f32 Easing::outSine(f32 t, f32 b, f32 c, f32 d)
{
	return c * sinf(t / d * (pi / 2)) + b;
}

f32 Easing::inOutSine(f32 t, f32 b, f32 c, f32 d)
{
	return -c / 2 * (cosf(pi * t / d) - 1) + b;
}

}