// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/assert.h"
#include "base/math/vec2_decl.h"

namespace base
{
template <typename Type>
Vec2Tpl<Type>::Vec2Tpl()
	: x(0.0f)
	, y(0.0f)
{}

template <typename Type>
Vec2Tpl<Type>::Vec2Tpl(const Vec2Tpl& other)
{
	x = other.x;
	y = other.y;
}

template <typename Type>
Vec2Tpl<Type>::Vec2Tpl(Type newX, Type newY)
{
	x = newX;
	y = newY;
}

template <typename Type>
Vec2Tpl<Type>::Vec2Tpl(Type value)
{
	x = value;
	y = value;
}

template <typename Type>
Type Vec2Tpl<Type>::dot(const Vec2Tpl& other) const
{
	return x * other.x + y * other.y;
}

template <typename Type>
Type Vec2Tpl<Type>::getDistance(const Vec2Tpl& other) const
{
	Type xx = x - other.x, yy = y - other.y;

	xx = xx * xx + yy * yy;

	if (xx <= 0.0f)
	{
		return 0.0f;
	}

	return (Type)sqrtf(xx);
}

template <typename Type>
void Vec2Tpl<Type>::makeAbsolute()
{
	x = fabs(x);
	y = fabs(y);
}

template <typename Type>
Type Vec2Tpl<Type>::getCos(const Vec2Tpl& other) const
{
	Type m = (x * x + y * y) * (other.x * other.x + other.y * other.y);

	if (m <= 0.0f)
	{
		return 0.0f;
	}

	return (Type)(x * other.x + y * other.y) / sqrt(m);
}

template <typename Type>
void Vec2Tpl<Type>::normalize()
{
	Type m = x * x + y * y;

	if (m <= 0.0f)
	{
		x = y = 0.0f;
		return;
	}

	m = sqrt(m);
	x /= m;
	y /= m;
}

template <typename Type>
Vec2Tpl<Type> Vec2Tpl<Type>::getNormalized() const
{
	Type m = x * x + y * y;
	Vec2Tpl value = *this;

	if (m <= 0.0f)
	{
		return Vec2Tpl();
	}

	m = sqrt((Type)m);
	value.x /= m;
	value.y /= m;

	return value;
}

template <typename Type>
Vec2Tpl<Type>& Vec2Tpl<Type>::normalizeTo(Vec2Tpl& to)
{
	Type m = x * x + y * y;

	if (m <= 0.0f)
	{
		return *this;
	}

	m = sqrt(m);
	to.x = x / m;
	to.y = y / m;

	return *this;
}

template <typename Type>
Vec2Tpl<Type> Vec2Tpl<Type>::getNegated() const
{
	Vec2Tpl value = *this;

	value.x = -x;
	value.y = -y;

	return value;
}

template <typename Type>
void Vec2Tpl<Type>::negate()
{
	x = -x;
	y = -y;
}

template <typename Type>
Vec2Tpl<Type>& Vec2Tpl<Type>::negateTo(Vec2Tpl& to)
{
	to.x = -x;
	to.y = -y;

	return *this;
}

template <typename Type>
Vec2Tpl<Type> Vec2Tpl<Type>::getRotated(const Vec2Tpl& origin, f32 angle) const
{
	B_ASSERT_NOT_IMPLEMENTED;
	return Vec2Tpl();
}

template <typename Type>
bool Vec2Tpl<Type>::isOnLine(const Vec2Tpl& lineA, const Vec2Tpl& lineB, Type tolerance) const
{
	Type u1, u2;

	u1 = (lineB.x - lineA.x);
	u2 = (lineB.y - lineA.y);

	if (u1 == 0.0f)
		u1 = 1.0f;

	if (u2 == 0.0f)
		u2 = 1.0f;

	u1 = (x - lineA.x) / u1;
	u2 = (y - lineA.y) / u2;

	return (fabs(u1 - u2) <= tolerance);
}

template <typename Type>
inline bool Vec2Tpl<Type>::isEquivalent(const Vec2Tpl& other, Type tolerance) const
{
	return fabs(x - other.x) <= tolerance
		&& fabs(y - other.y) <= tolerance;
}

template <typename Type>
inline Type Vec2Tpl<Type>::getLength() const
{
	return sqrt(x * x + y * y);
}

template <typename Type>
inline Type Vec2Tpl<Type>::getSquaredLength() const
{
	return x * x + y * y;
}

template <typename Type>
void Vec2Tpl<Type>::setLength(Type length)
{
	Type oldLen = getLength();

	if (oldLen < 0.0001f)
	{
		oldLen = 0.0001f;
	}

	Type l = length / oldLen;

	x *= l;
	y *= l;
}

template <typename Type>
inline Vec2Tpl<Type>& Vec2Tpl<Type>::set(Type newX, Type newY)
{
	x = newX;
	y = newY;

	return *this;
}

template <typename Type>
inline Vec2Tpl<Type>& Vec2Tpl<Type>::clear()
{
	x = y = 0.0f;

	return *this;
}

template <typename Type>
Type Vec2Tpl<Type>::operator [](int index) const
{
	if (index == 0)
	{
		return x;
	}
	else if (index == 1)
	{
		return y;
	}

	return 0;
}

template <typename Type>
inline Vec2Tpl<Type>& Vec2Tpl<Type>::operator += (const Vec2Tpl& value)
{
	x += value.x;
	y += value.y;

	return *this;
}

template <typename Type>
inline Vec2Tpl<Type>& Vec2Tpl<Type>::operator *= (const Vec2Tpl& value)
{
	x *= value.x;
	y *= value.y;

	return *this;
}

template <typename Type>
inline Vec2Tpl<Type>& Vec2Tpl<Type>::operator -= (const Vec2Tpl& value)
{
	x -= value.x;
	y -= value.y;

	return *this;
}

template <typename Type>
inline Vec2Tpl<Type>& Vec2Tpl<Type>::operator /= (const Vec2Tpl& value)
{
	x /= value.x;
	y /= value.y;

	return *this;
}

template <typename Type>
inline Vec2Tpl<Type>& Vec2Tpl<Type>::operator += (Type value)
{
	x += value;
	y += value;

	return *this;
}

template <typename Type>
inline Vec2Tpl<Type>& Vec2Tpl<Type>::operator *= (Type value)
{
	x *= value;
	y *= value;

	return *this;
}

template <typename Type>
inline Vec2Tpl<Type>& Vec2Tpl<Type>::operator -= (Type value)
{
	x -= value;
	y -= value;

	return *this;
}

template <typename Type>
inline Vec2Tpl<Type>& Vec2Tpl<Type>::operator /= (Type value)
{
	x /= value;
	y /= value;

	return *this;
}

template <typename Type>
inline Vec2Tpl<Type> Vec2Tpl<Type>::operator + (const Vec2Tpl& value) const
{
	Vec2Tpl result;

	result.x = x + value.x;
	result.y = y + value.y;

	return result;
}

template <typename Type>
inline Vec2Tpl<Type> Vec2Tpl<Type>::operator * (const Vec2Tpl& value) const
{
	Vec2Tpl result;

	result.x = x * value.x;
	result.y = y * value.y;

	return result;
}

template <typename Type>
inline Vec2Tpl<Type> Vec2Tpl<Type>::operator - (const Vec2Tpl& value) const
{
	Vec2Tpl result;

	result.x = x - value.x;
	result.y = y - value.y;

	return result;
}

template <typename Type>
inline Vec2Tpl<Type> Vec2Tpl<Type>::operator / (const Vec2Tpl& value) const
{
	Vec2Tpl result;

	result.x = x / value.x;
	result.y = y / value.y;

	return result;
}

template <typename Type>
inline Vec2Tpl<Type> Vec2Tpl<Type>::operator + (const Type value) const
{
	Vec2Tpl result;

	result.x = x + value;
	result.y = y + value;

	return result;
}

template <typename Type>
inline Vec2Tpl<Type> Vec2Tpl<Type>::operator * (const Type value) const
{
	Vec2Tpl result;

	result.x = x * value;
	result.y = y * value;

	return result;
}

template <typename Type>
inline Vec2Tpl<Type> Vec2Tpl<Type>::operator - (const Type value) const
{
	Vec2Tpl result;

	result.x = x - value;
	result.y = y - value;

	return result;
}

template <typename Type>
inline Vec2Tpl<Type> Vec2Tpl<Type>::operator / (const Type value) const
{
	Vec2Tpl result;

	result.x = x / value;
	result.y = y / value;

	return result;
}

template <typename Type>
inline Vec2Tpl<Type> Vec2Tpl<Type>::operator / (const size_t value) const
{
	Vec2Tpl result;

	result.x = x / (Type)value;
	result.y = y / (Type)value;

	return result;
}

template <typename Type>
inline Vec2Tpl<Type>& Vec2Tpl<Type>::operator = (const f32 value)
{
	x = value;
	y = value;

	return *this;
}

template <typename Type>
inline Vec2Tpl<Type>& Vec2Tpl<Type>::operator = (const i32 value)
{
	x = (f32)value;
	y = (f32)value;

	return *this;
}

template <typename Type>
inline bool Vec2Tpl<Type>::operator <= (const Vec2Tpl& other) const
{
	return x <= other.x
		&& y <= other.y;
}

template <typename Type>
inline bool Vec2Tpl<Type>::operator >= (const Vec2Tpl& other) const
{
	return x >= other.x
		&& y >= other.y;
}

template <typename Type>
inline bool Vec2Tpl<Type>::operator < (const Vec2Tpl& other) const
{
	return x < other.x
		&& y < other.y;
}

template <typename Type>
inline bool Vec2Tpl<Type>::operator > (const Vec2Tpl& other) const
{
	return x > other.x
		&& y > other.y;
}

template <typename Type>
inline bool Vec2Tpl<Type>::operator != (const Vec2Tpl& other) const
{
	return x != other.x
		&& y != other.y;
}

template <typename Type>
inline Vec2Tpl<Type>& Vec2Tpl<Type>::operator = (const Vec2Tpl& other)
{
	x = other.x;
	y = other.y;

	return *this;
}

typedef Vec2Tpl<f32> Vec2f;
typedef Vec2Tpl<i32> Vec2i;
typedef Vec2Tpl<u32> Vec2u;
typedef Vec2Tpl<f64> Vec2d;

#ifdef B_USE_DOUBLE_PRECISION_VEC2
typedef Vec2d Vec2;
#else
typedef Vec2f Vec2;
#endif

}