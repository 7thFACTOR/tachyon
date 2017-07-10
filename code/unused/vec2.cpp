#include "base/math/vec2.h"
#include "base/math/conversion.h"

namespace base
{
f32 Vec2::dot(const Vec2& rB) const
{
	return x * rB.x + y * rB.y;
}

f32 Vec2::distance(const Vec2& rB) const
{
	f32 xx = x - rB.x, yy = y - rB.y;

	xx = xx * xx + yy * yy;

	if (xx <= 0.0f)
	{
		return 0.0f;
	}

	return sqrtf(xx);
}

void Vec2::makeAbsolute()
{
	x = fabsf(x);
	y = fabsf(y);
}

f32 Vec2::cos(const Vec2& rB) const
{
	f32 m = (x * x + y * y) * (rB.x * rB.x + rB.y * rB.y);

	if (m <= 0.0f)
	{
		return 0.0f;
	}

	return (f32)(x * rB.x + y * rB.y) / sqrtf(m);
}

void Vec2::normalize()
{
	f32 m = x * x + y * y;

	if (m <= 0.0f)
	{
		x = y = 0.0f;
		return;
	}

	m = sqrtf(m);
	x /= m;
	y /= m;
}

Vec2 Vec2::normalized() const
{
	f32 m = x * x + y * y;
	Vec2 rV = *this;

	if (m <= 0.0f)
	{
		return Vec2();
	}

	m = sqrtf((f32)m);
	rV.x /= m;
	rV.y /= m;

	return rV;
}

Vec2& Vec2::normalizeTo(Vec2& rV)
{
	f32 m = x * x + y * y;

	if (m <= 0.0f)
	{
		return *this;
	}

	m = sqrtf(m);
	rV.x = x / m;
	rV.y = y / m;

	return *this;
}

Vec2 Vec2::negated() const
{
	Vec2 rV = *this;

	rV.x = -x;
	rV.y = -y;

	return rV;
}

void Vec2::negate()
{
	x = -x;
	y = -y;
}

Vec2& Vec2::negateTo(Vec2& rV)
{
	rV.x = - x;
	rV.y = - y;

	return *this;
}

Vec2 Vec2::rotate(const Vec2& rOrigin, f32 aAngle) const
{
	Quat q;
	Vec2 vec = *this - rOrigin;

	//TODO
	// toQuat(aAngle, aX, aY, aZ, q);
	// toMatrix(q).transform(vec, vec);
	// vec += rOrigin;

	return vec;
}

bool Vec2::isOnLine(const Vec2& rA, const Vec2& rB, f32 tolerance /* = 0.0001  */) const
{
	f32 u1, u2;

	u1 = (rB.x - rA.x);
	u2 = (rB.y - rA.y);

	if (u1 == 0.0f)
		u1 = 1.0f;

	if (u2 == 0.0f)
		u2 = 1.0f;

	u1 = (x - rA.x) / u1;
	u2 = (y - rA.y) / u2;

	return (fabsf(u1 - u2) <= tolerance);
}

}