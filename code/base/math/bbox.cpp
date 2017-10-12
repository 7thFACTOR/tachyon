// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "base/math/bbox.h"

namespace base
{
BBox::BBox()
{}

BBox::BBox(const BBox& other)
{
	min = other.min;
	max = other.max;
}

BBox& BBox::operator = (const BBox& other)
{
	min = other.min;
	max = other.max;

	return *this;
}

BBox::~BBox()
{}

void BBox::set(const Vec3& newMin, const Vec3& newMax)
{
	min = newMin;
	max = newMax;
}

f32 BBox::getWidth() const
{
	return max.x - min.x;
}

f32 BBox::getHeight() const
{
	return max.y - min.y;
}

f32 BBox::getDepth() const
{
	return max.z - min.z;
}

Vec3 BBox::getCenter() const
{
	return (max + min) / 2.0f;
}

void BBox::startBoundingBox()
{
	const f32 maxBBoxSize = 1000000000.0f;

	min.set(maxBBoxSize, maxBBoxSize, maxBBoxSize);
	max.set(-maxBBoxSize, -maxBBoxSize, -maxBBoxSize);
}

void BBox::addPoint(const Vec3& value)
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

void BBox::addPoints(const Vec3* array, u32 count)
{
	for (u32 i = 0; i < count; ++i)
	{
		addPoint(array[i]);
	}
}

void BBox::addBox(const BBox& value)
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

void BBox::getCorners(Vec3* corners, u32 count) const
{
	if (!corners)
		return;

	corners[0].set(min.x, min.y, min.z);
	corners[1].set(min.x, min.y, max.z);
	corners[2].set(max.x, min.y, max.z);
	corners[3].set(max.x, min.y, min.z);
	corners[4].set(min.x, max.y, min.z);
	corners[5].set(min.x, max.y, max.z);
	corners[6].set(max.x, max.y, max.z);
	corners[7].set(max.x, max.y, min.z);
}

bool BBox::isPointInBox(const Vec3& point) const
{
	if ((point.x >= min.x)
		&& (point.y >= min.y)
		&& (point.z >= min.z)
		&& (point.x <= max.x)
		&& (point.x <= max.y)
		&& (point.z <= max.z))
	{
		return true;
	}

	return false;
}

BBox BBox::operator * (f32 value)
{
	BBox box = *this;
	
	box.min *= value;
	box.max *= value;
	
	return box;
}

BBox BBox::operator + (f32 value)
{
	BBox box = *this;
	
	box.min += value;
	box.max += value;
	
	return box;
}

BBox BBox::operator - (f32 value)
{
	BBox box = *this;
	
	box.min -= value;
	box.max -= value;
	
	return box;
}

BBox BBox::operator / (f32 value)
{
	BBox box = *this;
	
	box.min /= value;
	box.max /= value;
	
	return box;
}

BBox BBox::operator * (const Vec3& value)
{
	BBox box = *this;
	
	box.min *= value;
	box.max *= value;
	
	return box;
}

BBox BBox::operator + (const Vec3& value)
{
	BBox box = *this;
	
	box.min += value;
	box.max += value;
	
	return box;
}

BBox BBox::operator - (const Vec3& value)
{
	BBox box = *this;
	
	box.min -= value;
	box.max -= value;
	
	return box;
}

BBox BBox::operator / (const Vec3& value)
{
	BBox box = *this;
	
	box.min /= value;
	box.max /= value;
	
	return box;
}

BBox& BBox::operator *= (f32 value)
{
	min *= value;
	max *= value;
	
	return *this;
}

BBox& BBox::operator += (f32 value)
{
	min += value;
	max += value;
	
	return *this;
}

BBox& BBox::operator -= (f32 value)
{
	min -= value;
	max -= value;
	
	return *this;
}

BBox& BBox::operator /= (f32 value)
{
	min /= value;
	max /= value;
	
	return *this;
}

BBox& BBox::operator *= (const Vec3& value)
{
	min *= value;
	max *= value;
	
	return *this;
}

BBox& BBox::operator += (const Vec3& value)
{
	min += value;
	max += value;
	
	return *this;
}

BBox& BBox::operator -= (const Vec3& value)
{
	min -= value;
	max -= value;
	
	return *this;
}

BBox& BBox::operator /= (const Vec3& value)
{
	min /= value;
	max /= value;
	
	return *this;
}

}