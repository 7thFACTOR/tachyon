#pragma once
#include "base/defines.h"
#include "base/math/bbox.h"
#include "base/math/matrix.h"
#include "base/math/vec3.h"
#include "base/math/ray.h"

namespace base
{
class B_API OrientedBBox
{
public:
	friend class BBox;

	OrientedBBox();
	OrientedBBox(const OrientedBBox& other);
	OrientedBBox& operator = (const OrientedBBox& other);
	~OrientedBBox();

	void set(const Vec3& min, const Vec3& max);
	void setMin(const Vec3& value);
	void setMax(const Vec3& value);
	const Vec3& getMin() const;
	const Vec3& getMax() const;
	f32 getWidth() const;
	f32 getHeight() const;
	f32 getDepth() const;
	Vec3 getCenter() const;
	void startBoundingBox();
	void getCorners(Vec3* corners, u32 count = 8) const;
	void addPoint(const Vec3& value);
	void addPoints(const Vec3* points, u32 count);
	void addBox(const OrientedBBox& value);
	void addBox(const BBox& value);
	bool isPointInBox(const Vec3& point) const;
	void setTransform(const Matrix& matrix);
	const Matrix& getTransformMatrix() const;
	const Matrix& getInverseTransformMatrix() const;
	void updateCornerPoints();

	OrientedBBox operator * (f32 value) const;
	OrientedBBox operator + (f32 value) const;
	OrientedBBox operator - (f32 value) const;
	OrientedBBox operator / (f32 value) const;
	OrientedBBox operator * (const Vec3& value) const;
	OrientedBBox operator + (const Vec3& value) const;
	OrientedBBox operator - (const Vec3& value) const;
	OrientedBBox operator / (const Vec3& value) const;
	OrientedBBox& operator *= (f32 value);
	OrientedBBox& operator += (f32 value);
	OrientedBBox& operator -= (f32 value);
	OrientedBBox& operator /= (f32 value);
	OrientedBBox& operator *= (const Vec3& value);
	OrientedBBox& operator += (const Vec3& value);
	OrientedBBox& operator -= (const Vec3& value);
	OrientedBBox& operator /= (const Vec3& value);

protected:
	Vec3 min, max;
	Vec3 corners[8];
	Matrix transform, inverseTransform;
};

}