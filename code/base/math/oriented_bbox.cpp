#include "base/math/oriented_bbox.h"

namespace base
{
OrientedBBox::OrientedBBox()
{
	min.set(-0.5f, -0.5f, -0.5f);
	max.set(0.5f, 0.5f, 0.5f);
}

OrientedBBox::OrientedBBox(const OrientedBBox& other)
{
	*this = other;
}

OrientedBBox& OrientedBBox::operator = (const OrientedBBox& other)
{
	min = other.min;
	max = other.max;

	return *this;
}

OrientedBBox::~OrientedBBox()
{
}

void OrientedBBox::set(const Vec3& newMin, const Vec3& newMax)
{
	min = newMin;
	max = newMax;
}

void OrientedBBox::setMin(const Vec3& value)
{
	min = value;
}

void OrientedBBox::setMax(const Vec3& value)
{
	max = value;
}

const Vec3& OrientedBBox::getMin() const
{
	return min;
}

const Vec3& OrientedBBox::getMax() const
{
	return max;
}

f32 OrientedBBox::getWidth() const
{
	return max.x - min.x;
}

f32 OrientedBBox::getHeight() const
{
	return max.y - min.y;
}

f32 OrientedBBox::getDepth() const
{
	return max.z - min.z;
}

Vec3 OrientedBBox::getCenter() const
{
	return (max + min) / 2.0f;
}

void OrientedBBox::startBoundingBox()
{
	const f32 maxBBoxSize = 1000000000.0f;

	min.set(maxBBoxSize, maxBBoxSize, maxBBoxSize);
	max.set(-maxBBoxSize, -maxBBoxSize, -maxBBoxSize);
}

void OrientedBBox::addPoint(const Vec3& value)
{
	if (value.x < min.x)
		min.x = value.x;

	if (value.x > max.x)
		max.x = value.x;

	if (value.y < min.y)
		min.y = value.y;

	if (value.y > max.y)
		max.y = value.y;

	if (value.z < min.z)
		min.z = value.z;

	if (value.z > max.z)
		max.z = value.z;
}

void OrientedBBox::addPoints(const Vec3* points, u32 count)
{
	for (u32 i = 0; i < count; ++i)
	{
		addPoint(points[i]);
	}
}

void OrientedBBox::addBox(const OrientedBBox& value)
{
	if (value.min.x < min.x)
		min.x = value.min.x;

	if (value.min.y < min.y)
		min.y = value.min.y;

	if (value.min.z < min.z)
		min.z = value.min.z;

	if (value.max.x > max.x)
		max.x = value.max.x;

	if (value.max.y > max.y)
		max.y = value.max.y;

	if (value.max.z > max.z)
		max.z = value.max.z;
}

void OrientedBBox::addBox(const BBox& value)
{
	if (value.getMinCorner().x < min.x)
		min.x = value.getMinCorner().x;

	if (value.getMinCorner().y < min.y)
		min.y = value.getMinCorner().y;

	if (value.getMinCorner().z < min.z)
		min.z = value.getMinCorner().z;

	if (value.getMaxCorner().x > max.x)
		max.x = value.getMaxCorner().x;

	if (value.getMaxCorner().y > max.y)
		max.y = value.getMaxCorner().y;

	if (value.getMaxCorner().z > max.z)
		max.z = value.getMaxCorner().z;
}

void OrientedBBox::getCorners(Vec3* outCorners, u32 count) const
{
	if (!outCorners)
		return;

	for (u32 i = 0; i < count; ++i)
	{
		outCorners[i] = corners[i];
	}
}

void OrientedBBox::setTransform(const Matrix& matrix)
{
	transform = matrix;
	inverseTransform = matrix;
	inverseTransform.invert();
}

const Matrix& OrientedBBox::getTransformMatrix() const
{
	return transform;
}

const Matrix& OrientedBBox::getInverseTransformMatrix() const
{
	return inverseTransform;
}

void OrientedBBox::updateCornerPoints()
{
	corners[0].set(min.x, min.y, min.z);
	corners[1].set(min.x, min.y, max.z);
	corners[2].set(max.x, min.y, max.z);
	corners[3].set(max.x, min.y, min.z);
	corners[4].set(min.x, max.y, min.z);
	corners[5].set(min.x, max.y, max.z);
	corners[6].set(max.x, max.y, max.z);
	corners[7].set(max.x, max.y, min.z);

	for (u32 i = 0; i < 8; ++i)
	{
		transform.transform(corners[i], corners[i]);
	}

	inverseTransform = transform.getInverted();
}

bool OrientedBBox::isPointInBox(const Vec3& point) const
{
	Vec3 pt = point;

	inverseTransform.transform(pt, pt);

	if ((pt.x >= min.x)
		&& (pt.y >= min.y)
		&& (pt.z >= min.z)
		&& (pt.x <= max.x)
		&& (pt.x <= max.y)
		&& (pt.z <= max.z))
	{
		return true;
	}

	return false;
}

OrientedBBox OrientedBBox::operator * (f32 value) const
{
	OrientedBBox box = *this;

	box.min *= value;
	box.max *= value;

	return box;
}

OrientedBBox OrientedBBox::operator + (f32 value) const
{
	OrientedBBox box = *this;

	box.min += value;
	box.max += value;

	return box;
}

OrientedBBox OrientedBBox::operator - (f32 value) const
{
	OrientedBBox box = *this;

	box.min -= value;
	box.max -= value;

	return box;
}

OrientedBBox OrientedBBox::operator / (f32 value) const
{
	OrientedBBox box = *this;

	box.min /= value;
	box.max /= value;

	return box;
}

OrientedBBox OrientedBBox::operator * (const Vec3& value) const
{
	OrientedBBox box = *this;

	box.min *= value;
	box.max *= value;

	return box;
}

OrientedBBox OrientedBBox::operator + (const Vec3& value) const
{
	OrientedBBox box = *this;

	box.min += value;
	box.max += value;

	return box;
}

OrientedBBox OrientedBBox::operator - (const Vec3& value) const
{
	OrientedBBox box = *this;

	box.min -= value;
	box.max -= value;

	return box;
}

OrientedBBox OrientedBBox::operator / (const Vec3& value) const
{
	OrientedBBox box = *this;

	box.min /= value;
	box.max /= value;

	return box;
}

OrientedBBox& OrientedBBox::operator *= (f32 value)
{
	min *= value;
	max *= value;

	return *this;
}

OrientedBBox& OrientedBBox::operator += (f32 value)
{
	min += value;
	max += value;

	return *this;
}

OrientedBBox& OrientedBBox::operator -= (f32 value)
{
	min -= value;
	max -= value;

	return *this;
}

OrientedBBox& OrientedBBox::operator /= (f32 value)
{
	min /= value;
	max /= value;

	return *this;
}

OrientedBBox& OrientedBBox::operator *= (const Vec3& value)
{
	min *= value;
	max *= value;

	return *this;
}

OrientedBBox& OrientedBBox::operator += (const Vec3& value)
{
	min += value;
	max += value;

	return *this;
}

OrientedBBox& OrientedBBox::operator -= (const Vec3& value)
{
	min -= value;
	max -= value;

	return *this;
}

OrientedBBox& OrientedBBox::operator /= (const Vec3& value)
{
	min /= value;
	max /= value;

	return *this;
}

}