#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/math/vec3.h"
#include "base/math/ray.h"

namespace base
{
class B_API BBox
{
public:
	enum class ClipType
	{
		Outside = B_BIT(0),
		IsEqual = B_BIT(1),
		IsContained = B_BIT(2),
		Contains = B_BIT(3),
		Clips = B_BIT(4)
	};

	BBox();
	BBox(const BBox& other);
	BBox& operator = (const BBox& other);
	~BBox();

	void set(const Vec3& min, const Vec3& max);
	f32 getWidth() const;
	f32 getHeight() const;
	f32 getDepth() const;
	Vec3 getCenter() const;
	const Vec3& getMinCorner() const { return min; }
	const Vec3& getMaxCorner() const { return max; }
	void startBoundingBox();
	void getCorners(Vec3* outCorners, u32 count = 8) const;
	void addPoint(const Vec3& value);
	void addPoints(const Vec3* pArray, u32 count);
	void addBox(const BBox& value);
	bool isPointInBox(const Vec3& point) const;

	BBox operator * (f32 value);
	BBox operator + (f32 value);
	BBox operator - (f32 value);
	BBox operator / (f32 value);
	BBox operator * (const Vec3& value);
	BBox operator + (const Vec3& value);
	BBox operator - (const Vec3& value);
	BBox operator / (const Vec3& value);
	BBox& operator *= (f32 value);
	BBox& operator += (f32 value);
	BBox& operator -= (f32 value);
	BBox& operator /= (f32 value);
	BBox& operator *= (const Vec3& value);
	BBox& operator += (const Vec3& value);
	BBox& operator -= (const Vec3& value);
	BBox& operator /= (const Vec3& value);

	Vec3 min, max;
};

}