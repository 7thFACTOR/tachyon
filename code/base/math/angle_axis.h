#pragma once
#include "base/defines.h"
#include "base/math/vec3.h"
#include "base/math/quat.h"
#include "base/math/matrix.h"

namespace base
{
class B_API AngleAxis
{
public:
	AngleAxis();
	~AngleAxis();
	AngleAxis(f32 angle, f32 x, f32 y, f32 z);
	AngleAxis(f32 angle, const Vec3& axis);
	AngleAxis(const AngleAxis& angleAxis);
	AngleAxis(const Matrix& matrix);
	AngleAxis(const Quat& quat);

	void set(f32 angle, f32 x, f32 y, f32 z);
	void set(f32 angle, const Vec3& axis);
	void set(const AngleAxis& angleAxis);
	void set(const Matrix& matrix);
	void set(const Quat& quat);

	inline AngleAxis& operator = (const AngleAxis& other)
	{
		angle = other.angle;
		axis = other.axis;

		return *this;
	}

	f32 angle = 0;
	Vec3 axis;
};

}