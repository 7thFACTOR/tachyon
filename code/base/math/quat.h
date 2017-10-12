// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"
#include <math.h>
#include "base/math/types.h"
#include "base/math/vec3.h"
#include "base/math/matrix_decl.h"
#include "base/math/euler.h"

namespace base
{
class AngleAxis;
typedef MatrixTpl<f32> Matrix;

class B_API Quat
{
public:
	Quat();
	~Quat();

	Quat(const Quat& other);
	Quat(const Euler& euler, Euler::RotationOrder order);
	Quat(f32 angle, f32 x, f32 y, f32 z);
	Quat(f32 angle, const Vec3& axis);
	Quat(const AngleAxis& angleAxis);
	Quat(const Matrix& matrix);

	Quat& reset();
	Quat& set(const Quat& other);
	Quat& setMembers(f32 x, f32 y, f32 z, f32 w);
	Quat& postMultiply(const Quat& quat);
	Quat& multiplyAndSet(const Quat& quat1, const Quat& quat2);
	void setQuatFromDirectionAndUpAxis(const Vec3& direction, const Vec3& up);
	void square(Quat& quat);
	void sqr(Quat& quat);
	f32 getLength() const;
	f32 dot(const Quat& quat) const;
	f32 dotUnit(const Quat& quat) const;
	void scale(f32 scale);
	void rescale(f32 scale);
	void normalize();
	void invert();
	void negate();
	void exp();
	void log();
	void lnDif(const Quat& quat);
	void slerp(const Quat& quatA, const Quat& quatB, f32 spin, f32 alpha);
	void slerp(const Quat& quatA, const Quat& quatB, f32 time);
	void multiply(f32 value);
	void blend(f32 factor, const Quat& quat);

	Quat operator + (const Quat& other) const;
	Quat operator - (const Quat& other) const;
	Quat operator * (const Quat& other) const;
	Quat operator / (const Quat& other) const;

	Quat& operator += (const Quat& other);
	Quat& operator -= (const Quat& other);
	Quat& operator *= (const Quat& other);
	Quat& operator /= (const Quat& other);

	Quat& operator += (f32 value);
	Quat& operator -= (f32 value);
	Quat& operator *= (f32 value);
	Quat& operator /= (f32 value);

	inline bool operator == (const Quat& other) const
	{
		const f32 epsilon = 0.0001f;

		return fabs(x - other.x) < epsilon
			&& fabs(y - other.y) < epsilon
			&& fabs(z - other.z) < epsilon
			&& fabs(w - other.w) < epsilon;
	}

	inline bool operator <= (const Quat& other) const
	{
		return x <= other.x
			&& y <= other.y
			&& z <= other.z
			&& w <= other.w;
	}

	inline bool operator >= (const Quat& other) const
	{
		return x >= other.x
			&& y >= other.y
			&& z >= other.z
			&& w >= other.w;
	}

	inline bool operator < (const Quat& other) const
	{
		return x < other.x
			&& y < other.y
			&& z < other.z
			&& w < other.w;
	}

	inline bool operator > (const Quat& other) const
	{
		return x > other.x
			&& y > other.y
			&& z > other.z
			&& w > other.w;
	}

	inline bool operator != (const Quat& other) const
	{
		const f32 epsilon = 0.0001f;

		return fabs(x - other.x) > epsilon
			|| fabs(y - other.y) > epsilon
			|| fabs(z - other.z) > epsilon
			|| fabs(w - other.w) > epsilon;
	}

	inline Quat& operator = (const Quat& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		w = other.w;

		return *this;
	}

	f32 x, y, z, w;
};

}