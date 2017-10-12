// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/math/vec3.h"

namespace base
{
class B_API Ray
{
public:
	Ray();
	Ray(const Ray& other);
	Ray(const Vec3& start, const Vec3& end);
	~Ray();

	void set(const Vec3& start, const Vec3& end);
	const Vec3& getStartPoint() const;
	Vec3 getEndPoint() const;
	Vec3 getEndPoint(f32 length) const;
	const Vec3& getDirection() const;
	void setStart(const Vec3& value);
	void setEnd(const Vec3& value);
	void setDirection(const Vec3& value);
	f32 getLength() const;
	f32 getDistanceTo(const Vec3& point) const;
	Vec3 getPointAtTime(f32 time) const;

	inline Ray& operator = (const Ray& other)
	{
		direction = other.direction;
		start = other.start;

		return *this;
	}

private:
	Vec3 direction, start;
};

}