#pragma once
#include <stddef.h>
#include <math.h>
#include "base/defines.h"
#include "base/types.h"
#include "base/math/vec3_decl.h"

namespace base
{
template <typename Type>
Vec3Tpl<Type>::Vec3Tpl()
	: x(0.0f)
	, y(0.0f)
	, z(0.0f)
{}

template <typename Type>
Vec3Tpl<Type>::Vec3Tpl(const Vec3Tpl& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
}

template <typename Type>
Vec3Tpl<Type>::Vec3Tpl(Type newX, Type newY, Type newZ)
{
	x = newX;
	y = newY;
	z = newZ;
}

template <typename Type>
Vec3Tpl<Type>::Vec3Tpl(Type value)
{
	x = value;
	y = value;
	z = value;
}

template <typename Type>
Vec3Tpl<Type> Vec3Tpl<Type>::cross(const Vec3Tpl& other) const
{
	return Vec3Tpl(
		y * other.z - z * other.y,
		z * other.x - x * other.z,
		x * other.y - y * other.x);
}

template <typename Type>
Type Vec3Tpl<Type>::dot(const Vec3Tpl& other) const
{
	return x * other.x + y * other.y + z * other.z;
}

template <typename Type>
Type Vec3Tpl<Type>::getDistance(const Vec3Tpl& to) const
{
	Type xx = x - to.x, yy = y - to.y, zz = z - to.z;

	xx = xx * xx + yy * yy + zz * zz;

	if (xx <= 0.00001f)
	{
		return 0.0f;
	}

	return (Type)sqrt(xx);
}

template <typename Type>
inline void Vec3Tpl<Type>::makeAbsolute()
{
	x = fabs(x);
	y = fabs(y);
	z = fabs(z);
}

template <typename Type>
Type Vec3Tpl<Type>::getCosAngle(const Vec3Tpl& other) const
{
	Type m = (x * x + y * y + z * z) * (other.x * other.x + other.y * other.y + other.z * other.z);

	if (m <= 0.00001f)
	{
		return 0.0f;
	}

	return (Type)(x * other.x + y * other.y + z * other.z) / sqrt(m);
}

template <typename Type>
void Vec3Tpl<Type>::normalize()
{
	Type m = x * x + y * y + z * z;

	if (m <= 0.00001f)
	{
		x = y = z = 0.0f;
		return;
	}

	m = sqrt(m);
	x /= m;
	y /= m;
	z /= m;
}

template <typename Type>
Vec3Tpl<Type> Vec3Tpl<Type>::getNormalized() const
{
	Type m = x * x + y * y + z * z;
	Vec3Tpl value = *this;

	if (m <= 0.00001f)
	{
		return Vec3Tpl();
	}

	m = sqrt(m);
	value.x /= m;
	value.y /= m;
	value.z /= m;

	return value;
}

template <typename Type>
Vec3Tpl<Type>& Vec3Tpl<Type>::normalizeTo(Vec3Tpl& to)
{
	Type m = x * x + y * y + z * z;

	if (m <= 0.00001f)
	{
		return *this;
	}

	m = sqrt(m);
	to.x = x / m;
	to.y = y / m;
	to.z = z / m;

	return *this;
}

template <typename Type>
Vec3Tpl<Type> Vec3Tpl<Type>::getNegated() const
{
	Vec3Tpl value = *this;

	value.x = -x;
	value.y = -y;
	value.z = -z;

	return value;
}

template <typename Type>
void Vec3Tpl<Type>::negate()
{
	x = -x;
	y = -y;
	z = -z;
}

template <typename Type>
Vec3Tpl<Type>& Vec3Tpl<Type>::negateTo(Vec3Tpl& to)
{
	to.x = -x;
	to.y = -y;
	to.z = -z;

	return *this;
}

template <typename Type>
bool Vec3Tpl<Type>::isOnLine(const Vec3Tpl& lineA, const Vec3Tpl& lineB, Type tolerance) const
{
	Type u1, u2;

	u1 = (lineB.x - lineA.x);
	u2 = (lineB.y - lineA.y);

	if (fabs(u1) <= 0.00001f)
		u1 = 1.0f;

	if (fabs(u2) <= 0.00001f)
		u2 = 1.0f;

	u1 = (x - lineA.x) / u1;
	u2 = (y - lineA.y) / u2;

	return (fabs(u1 - u2) <= tolerance);
}

template <typename Type>
bool Vec3Tpl<Type>::isOnSameSide(const Vec3Tpl& p1, const Vec3Tpl& p2, const Vec3Tpl& a, const Vec3Tpl& b)
{
	Vec3Tpl cp1 = (b - a).cross(p1 - a);
	Vec3Tpl cp2 = (b - a).cross(p2 - a);

	if (cp1.dot(cp2) >= 0.0f)
		return true;

	return false;
}

template <typename Type>
bool Vec3Tpl<Type>::isInTriangle(const Vec3Tpl& vertexPos1, const Vec3Tpl& vertexPos2, const Vec3Tpl& vertexPos3) const
{
	if (isOnSameSide(*this, vertexPos1, vertexPos2, vertexPos3)
		&& isOnSameSide(*this, vertexPos2, vertexPos1, vertexPos3)
		&& isOnSameSide(*this, vertexPos3, vertexPos1, vertexPos2))
	{
		return true;
	}

	return false;
}
	
template <typename Type>
inline bool Vec3Tpl<Type>::isEquivalent(const Vec3Tpl& other, Type tolerance) const
{
	return fabs(x - other.x) <= tolerance
		&& fabs(y - other.y) <= tolerance
		&& fabs(z - other.z) <= tolerance;
}

template <typename Type>
inline Type Vec3Tpl<Type>::getLength() const
{
	return sqrt(x * x + y * y + z * z);
}

template <typename Type>
inline Type Vec3Tpl<Type>::getSquaredLength() const
{
	return x * x + y * y + z * z;
}

template <typename Type>
void Vec3Tpl<Type>::setLength(Type length)
{
	Type oldLen = getLength();

	if (oldLen <= 0.00001f)
	{
		oldLen = 0.0001f;
	}

	Type l = length / oldLen;

	x *= l;
	y *= l;
	z *= l;
}

template <typename Type>
inline Vec3Tpl<Type>& Vec3Tpl<Type>::set(Type newX, Type newY, Type newZ)
{
	x = newX;
	y = newY;
	z = newZ;

	return *this;
}

template <typename Type>
inline Vec3Tpl<Type>& Vec3Tpl<Type>::makeZero()
{
	x = y = z = 0.0f;

	return *this;
}

template <typename Type>
Type Vec3Tpl<Type>::operator [](int index) const
{
	if (index == 0)
	{
		return x;
	}
	else if (index == 1)
	{
		return y;
	}
	else if (index == 2)
	{
		return z;
	}

	return 0;
}

template <typename Type>
inline Vec3Tpl<Type>& Vec3Tpl<Type>::operator += (const Vec3Tpl& value)
{
	x += value.x;
	y += value.y;
	z += value.z;

	return *this;
}

template <typename Type>
inline Vec3Tpl<Type>& Vec3Tpl<Type>::operator *= (const Vec3Tpl& value)
{
	x *= value.x;
	y *= value.y;
	z *= value.z;

	return *this;
}

template <typename Type>
inline Vec3Tpl<Type>& Vec3Tpl<Type>::operator -= (const Vec3Tpl& value)
{
	x -= value.x;
	y -= value.y;
	z -= value.z;

	return *this;
}

template <typename Type>
inline Vec3Tpl<Type>& Vec3Tpl<Type>::operator /= (const Vec3Tpl& value)
{
	x /= value.x;
	y /= value.y;
	z /= value.z;

	return *this;
}

template <typename Type>
inline Vec3Tpl<Type>& Vec3Tpl<Type>::operator += (Type value)
{
	x += value;
	y += value;
	z += value;

	return *this;
}

template <typename Type>
inline Vec3Tpl<Type>& Vec3Tpl<Type>::operator *= (Type value)
{
	x *= value;
	y *= value;
	z *= value;

	return *this;
}

template <typename Type>
inline Vec3Tpl<Type>& Vec3Tpl<Type>::operator -= (Type value)
{
	x -= value;
	y -= value;
	z -= value;

	return *this;
}

template <typename Type>
inline Vec3Tpl<Type>& Vec3Tpl<Type>::operator /= (Type value)
{
	x /= value;
	y /= value;
	z /= value;

	return *this;
}

template <typename Type>
inline Vec3Tpl<Type> Vec3Tpl<Type>::operator + (const Vec3Tpl& value) const
{
	Vec3Tpl result;

	result.x = x + value.x;
	result.y = y + value.y;
	result.z = z + value.z;

	return result;
}

template <typename Type>
inline Vec3Tpl<Type> Vec3Tpl<Type>::operator * (const Vec3Tpl& value) const
{
	Vec3Tpl result;

	result.x = x * value.x;
	result.y = y * value.y;
	result.z = z * value.z;

	return result;
}

template <typename Type>
inline Vec3Tpl<Type> Vec3Tpl<Type>::operator - (const Vec3Tpl& value) const
{
	Vec3Tpl result;

	result.x = x - value.x;
	result.y = y - value.y;
	result.z = z - value.z;

	return result;
}

template <typename Type>
inline Vec3Tpl<Type> Vec3Tpl<Type>::operator / (const Vec3Tpl& value) const
{
	Vec3Tpl result;

	result.x = x / value.x;
	result.y = y / value.y;
	result.z = z / value.z;

	return result;
}

template <typename Type>
inline Vec3Tpl<Type> Vec3Tpl<Type>::operator + (const Type value) const
{
	Vec3Tpl result;

	result.x = x + value;
	result.y = y + value;
	result.z = z + value;

	return result;
}

template <typename Type>
inline Vec3Tpl<Type> Vec3Tpl<Type>::operator * (const Type value) const
{
	Vec3Tpl result;

	result.x = x * value;
	result.y = y * value;
	result.z = z * value;

	return result;
}

template <typename Type>
inline Vec3Tpl<Type> Vec3Tpl<Type>::operator - (const Type value) const
{
	Vec3Tpl result;

	result.x = x - value;
	result.y = y - value;
	result.z = z - value;

	return result;
}

template <typename Type>
inline Vec3Tpl<Type> Vec3Tpl<Type>::operator / (const Type value) const
{
	Vec3Tpl result;

	result.x = x / value;
	result.y = y / value;
	result.z = z / value;

	return result;
}

template <typename Type>
inline Vec3Tpl<Type> Vec3Tpl<Type>::operator / (const i32 value) const
{
	Vec3Tpl result;

	result.x = x / (Type)value;
	result.y = y / (Type)value;
	result.z = z / (Type)value;

	return result;
}

template <typename Type>
inline Vec3Tpl<Type> Vec3Tpl<Type>::operator / (const size_t value) const
{
	Vec3Tpl result;

	result.x = x / (Type)value;
	result.y = y / (Type)value;
	result.z = z / (Type)value;

	return result;
}

template <typename Type>
inline Vec3Tpl<Type>& Vec3Tpl<Type>::operator = (const Type value)
{
	x = value;
	y = value;
	z = value;

	return *this;
}

template <typename Type>
inline Vec3Tpl<Type>& Vec3Tpl<Type>::operator = (const i32 value)
{
	x = (Type)value;
	y = (Type)value;
	z = (Type)value;

	return *this;
}

template <typename Type>
inline bool Vec3Tpl<Type>::operator <= (const Vec3Tpl& other) const
{
	return x <= other.x
		&& y <= other.y
		&& z <= other.z;
}

template <typename Type>
inline bool Vec3Tpl<Type>::operator >= (const Vec3Tpl& other) const
{
	return x >= other.x
		&& y >= other.y
		&& z >= other.z;
}

template <typename Type>
inline bool Vec3Tpl<Type>::operator < (const Vec3Tpl& other) const
{
	return x < other.x
		&& y < other.y
		&& z < other.z;
}

template <typename Type>
inline bool Vec3Tpl<Type>::operator > (const Vec3Tpl& other) const
{
	return x > other.x
		&& y > other.y
		&& z > other.z;
}

template <typename Type>
inline bool Vec3Tpl<Type>::operator != (const Vec3Tpl& other) const
{
	return x != other.x
		&& y != other.y
		&& z != other.z;
}

typedef Vec3Tpl<f32> Vec3f;
typedef Vec3Tpl<i32> Vec3i;
typedef Vec3Tpl<f64> Vec3d;
typedef Vec3Tpl<f64> BigVec3; //!< used for 64bit 3D world positions

#ifdef B_USE_DOUBLE_PRECISION_VEC3
typedef Vec3d Vec3;
#else
typedef Vec3f Vec3;
#endif

}