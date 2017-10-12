// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include <math.h>
#include <float.h>
#include "base/defines.h"
#include "base/types.h"
#include "base/math/vec3.h"
#include "base/math/rect.h"
#include "base/math/matrix.h"

namespace base
{
class Quat;
class BBox;
class Ray;
class Plane;

template<typename Type>
inline void swapValues(Type& a, Type& b)
{
	Type tmp = a;
	a = b;
	b = tmp;
}

template<typename Type>
inline Type clampValue(Type a, Type min, Type max)
{
	if (a < min)
	{
		return min;
	}

	if (a > max)
	{
		return max;
	}

	return a;
}

template<typename Type>
inline const Type& minValue(const Type& left, const Type& right)
{
	return right < left ? right : left;
}

template<typename Type>
inline const Type& maxValue(const Type& left, const Type& right)
{
	return right > left ? right : left;
}

template<typename Type>
inline bool almostEqual(const Type& left, const Type& right, const Type& tolerance)
{
	return right > left ? (right - left) <= tolerance : (left - right) <= tolerance;
}

B_EXPORT f32 cos2D(f32 x1, f32 y1, f32 x2, f32 y2);

B_EXPORT Vec3 hermitePoint(
	const Vec3& controlPointA,
	const Vec3& tangentA, const Vec3& tangentB,
	const Vec3& controlPointB,
	f32 time);

B_EXPORT void reflectedPoint(
	const Vec3& normal,
	const Vec3& ray,
	Vec3& outReflected);

B_EXPORT void refractedPoint(
	const Vec3& normal,
	const Vec3& ray,
	Vec3& outRefracted,
	f32 coefficient);

B_EXPORT void projectPointOnPlane(
	const Vec3& planePt1,
	const Vec3& planePt2,
	const Vec3& planePt3,
	const Vec3& point,
	Vec3& outProjPoint);

B_EXPORT void setRandomSeed(i32 seed);
B_EXPORT i32 getRandomSeed();
B_EXPORT i32 randomInteger(i32 max);
B_EXPORT f32 randomFloat(f32 low, f32 high);
B_EXPORT f32 randomNormalizedFloat();

B_EXPORT Vec3 closestPointOnSegment(
	const Vec3& segmentPtA,
	const Vec3& segmentPtB,
	const Vec3& fromPoint,
	bool& outIsOnEdge);

B_EXPORT Vec3 closestPointOnLine(
	const Vec3& linePtA,
	const Vec3& linePtB,
	const Vec3& fromPoint);

B_EXPORT bool isPointInsideEdge(
	const Vec3& linePtA,
	const Vec3& linePtB,
	const Vec3& point);

B_EXPORT void projectPoints(
	const Vec3* pInPoints,
	u32 count,
	Vec3* outPoints,
	const Rect& viewport,
	const Matrix& world,
	const Matrix& view,
	const Matrix& projection);

B_EXPORT void unProjectPoints(
	const Vec3* inPoints,
	u32 count,
	Vec3* outPoints,
	const Rect& viewport,
	const Matrix& world,
	const Matrix& view,
	const Matrix& projection);

B_EXPORT Ray computePickRay(
	i32 screenX,
	i32 screenY,
	const Rect& viewport,
	const Matrix& world,
	const Matrix& view,
	const Matrix& projection);

}