// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/array.h"
#include "base/types.h"
#include "base/math/vec3.h"
#include "base/math/bbox.h"
#include "base/math/matrix.h"

namespace base
{
class Quat;
class BBox;
class Ray;
class Plane;
class OrientedBBox;

B_EXPORT bool intersectSphereSphere(
	const Vec3& sphereCenter1, const Vec3& sphereCenter2,
	const Vec3& velocity1, const Vec3& velocity2,
	f32 radius1, f32 radius2,
	f32& outTimeIn, f32& outTimeOut);

B_EXPORT bool intersectTriangleLine(
	const Vec3& p1, const Vec3& p2, const Vec3& p3,
	const Vec3& l1, const Vec3& l2,
	Vec3& outIntersection, f32& outTime,
	bool acceptBackface = true);

B_EXPORT bool intersectLineLine(
	const Vec3& la1, const Vec3& la2,
	const Vec3& lb1, const Vec3& lb2,
	Vec3& outL1, Vec3& outL2,
	f32& outT1, f32& outT2,
	f32 tolerance = 0.0001f);

B_EXPORT bool intersectSegmentSegment(
	const Vec3& la1, const Vec3& la2,
	const Vec3& lb1, const Vec3& lb2,
	Vec3& outL1, Vec3& outL2,
	f32& outT1, f32& outT2,
	f32 smallestDistance = 0,
	f32 tolerance = 0.0001f);

B_EXPORT bool intersectLinePolyline(
	const Vec3& lineA, const Vec3& lineB,
	const Array<Vec3>& polyline,
	f32 smallestDistance = 1.0f,
	f32 tolerance = 0.0001f);

B_EXPORT bool intersectRaySphere(
	const Vec3& center,
	f32 radius,
	const Ray& ray,
	f32& outHitTime,
	Vec3& outHitPosIn,
	Vec3& outHitPosOut);

B_EXPORT BBox::ClipType intersectBoxBox(const BBox& box1, const BBox& box2);

B_EXPORT bool intersectBoxRay(
	const BBox& box,
	const Ray& line,
	Vec3& outPoint,
	Vec3& outNormal,
	f32& outTime);

B_EXPORT bool intersectOrientedBoxRay(
	const OrientedBBox& box,
	const Ray& line,
	Vec3& outPoint,
	Vec3& outNormal,
	f32& outTime);

B_EXPORT bool intersectPlaneRay(const Plane& plane, const Ray& ray, f32& outTime);
B_EXPORT bool intersectPlanePlane(const Plane& p1, const Plane& p2, Ray& outRay);

}