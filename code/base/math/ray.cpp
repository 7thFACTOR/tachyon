#include <math.h>
#include "base/math/ray.h"
#include "base/math/util.h"

namespace base
{
Ray::Ray()
{}

Ray::Ray(const Ray& other)
{
	*this = other;
}

Ray::~Ray()
{}

Ray::Ray(const Vec3& start, const Vec3& end)
{
	set(start, end);
}

void Ray::set(const Vec3& newStart, const Vec3& newEnd)
{
	direction = newEnd - newStart;
	start = newStart;
}

const Vec3& Ray::getStartPoint() const
{
	return start;
}

Vec3 Ray::getEndPoint() const
{
	return(start + direction);
}

Vec3 Ray::getEndPoint(f32 length) const
{
	return(start + direction.getNormalized() * length);
}

const Vec3& Ray::getDirection() const
{
	return direction;
}

void Ray::setStart(const Vec3& value)
{
	start = value;
}

void Ray::setEnd(const Vec3& value)
{
	direction = value - start;
}

void Ray::setDirection(const Vec3& value)
{
	direction = value;
}

f32 Ray::getLength() const
{
	return direction.getLength();
}

f32 Ray::getDistanceTo(const Vec3& point) const
{
	Vec3 diff(point - start);
	f32 l = direction.dot(direction);

	if (l > 0.0f)
	{
		f32 t = direction.dot(diff) / l;
		diff = diff - direction * t;
		return diff.getLength();
	}
	else
	{
		Vec3 v(point - start);
		return v.getLength();
	}
}

Vec3 Ray::getPointAtTime(f32 time) const
{
	return Vec3(start + direction * time);
}

}