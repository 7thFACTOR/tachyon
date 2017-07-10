#pragma once
#include "base/defines.h"
#include "base/math/vec3.h"
#include "base/math/ray.h"

namespace base
{
class B_API Plane
{
public:
	Plane()
		: a(0.0f)
		, b(0.0f)
		, c(0.0f)
		, d(0.0f)
	{}

	Plane(const Vec3& A, const Vec3& B, const Vec3& C)
	{
		set(A, B, C);
	}

	Plane(const Vec3& newOrigin, const Vec3& normal)
	{
		setOriginNormal(newOrigin, normal);
	}

	void set(const Vec3& A, const Vec3& B, const Vec3& C)
	{
		Vec3 edge = C - A;
		Vec3 cross = edge.cross(B - A);

		cross.normalize();
		origin = A;
		a = cross.x;
		b = cross.y;
		c = cross.z;
		d = -(a * A.x + b * A.y + c * A.z);
	}

	void setOriginNormal(const Vec3& newOrigin, const Vec3& normal)
	{
		a = normal.x;
		b = normal.y;
		c = normal.z;
		d = -newOrigin.dot(normal);
		origin = newOrigin;
	}

	void setOrigin(const Vec3& newOrigin)
	{
		d = -newOrigin.dot(getNormal());
		origin = newOrigin;
	}

	void setNormal(const Vec3& normal)
	{
		a = normal.x;
		b = normal.y;
		c = normal.z;
		d = -origin.dot(normal);
	}

	inline void set(f32 A, f32 B, f32 C, f32 D)
	{
		a = A;
		b = B;
		c = C;
		d = D;
	}

	void normalize()
	{
		f32 invlen = 1.0f / sqrtf(a * a + b * b + c * c);

		a *= invlen;
		b *= invlen;
		c *= invlen;
		d *= invlen;
	}

	Vec3 getClosestPointTo(const Vec3& point) const
	{
		f32 t = getDistance(point);
		return Vec3(point - getNormal() * t);
	}

	f32 getDistance(const Vec3& point) const
	{
		f32 m = ::sqrt(a * a + b * b + c * c);

		if (m < 0.0001f)
		{
			return 0.0f;
		}

		return (a * point.x + b * point.y + c * point.z + d) / m;
	}

	f32 getSquaredDistance(const Vec3& point) const
	{
		return (a * point.x + b * point.y + c * point.z + d);
	}

	inline Vec3 getNormal() const
	{
		return Vec3(a, b, c);
	}

	f32 a, b, c, d;
	Vec3 origin;
};

}