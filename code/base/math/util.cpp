// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include <stdlib.h>
#include "base/math/util.h"
#include "base/math/vec3.h"
#include "base/math/plane.h"
#include "base/math/ray.h"
#include "base/math/matrix.h"
#include "base/math/intersection.h"
#include "base/math/rect.h"

namespace base
{
static u32 mathRandomSeed = 500;
const f64 mathEpsilon = 0.00001f;

f32 cos2D(f32 x1, f32 y1, f32 x2, f32 y2)
{
	f32 m = (x1 * x1 + y1 * y1) * (x2 * x2 + y2 * y2);

	if (m <= 0.0f)
		return 0.0f;

	return (f32)(x1 * x2 + y1 * y2) / sqrtf((f32)m);
}

Vec3 hermitePoint(
	const Vec3& controlPtA,
	const Vec3& tangentA,
	const Vec3& tangentB,
	const Vec3& controlPtB,
	f32 time)
{
	f32 t1, t2, t3, t4, tp3, tp2;
	Vec3 v;

	tp3 = time * time * time;
	tp2 = time * time;

	t1 = 2.0f * tp3 - 3.0f * tp2 + 1.0f;
	t2 = tp3 - 2.0f * tp2 + time;
	t3 = (-2.0f * tp3) + 3.0f * tp2;
	t4 = tp3 - tp2;

	v.x = controlPtA.x * t1 + tangentA.x * t2 + controlPtB.x * t3 - tangentB.x * t4;
	v.y = controlPtA.y * t1 + tangentA.y * t2 + controlPtB.y * t3 - tangentB.y * t4;
	v.z = controlPtA.z * t1 + tangentA.z * t2 + controlPtB.z * t3 - tangentB.z * t4;

	return v;
}

void reflectedPoint(const Vec3& normal, const Vec3& ray, Vec3& outReflected)
{
	f32 tp;

	tp = (f32) ray.getCosAngle(normal) * ray.getLength() / normal.getLength();

	outReflected.x = 2.0f * tp * normal.x - ray.x;
	outReflected.y = 2.0f * tp * normal.y - ray.y;
	outReflected.z = 2.0f * tp * normal.z - ray.z;
}

void refractedPoint(const Vec3& normal, const Vec3& ray, Vec3& outRefracted, f32 coefficient)
{
	f32 tp;
	Vec3 p;

	tp = (ray.getCosAngle(normal) * ray.getLength()) / normal.getLength();
	p.set(-tp * normal.x, -tp * normal.y, -tp * normal.z);

	outRefracted.x = p.x + coefficient * (p.x - ray.x);
	outRefracted.y = p.y + coefficient * (p.y - ray.y);
	outRefracted.z = p.z + coefficient * (p.z - ray.z);
}

void projectPointOnPlane(
	const Vec3& planePt1,
	const Vec3& planePt2,
	const Vec3& planePt3,
	const Vec3& point,
	Vec3& outProjectedPt)
{
	Plane plane(planePt1, planePt2, planePt3);
	f32 t;
	Vec3 normal = plane.getNormal();

	normal.negate();
	Ray ray(point, normal);
	intersectPlaneRay(plane, ray, t);
	outProjectedPt = ray.getPointAtTime(1.0f - t);
}

void setRandomSeed(i32 seed)
{
	mathRandomSeed = seed;
	srand(seed);
}

i32 getRandomSeed()
{
	return mathRandomSeed;
}

i32 randomInteger(i32 max)
{
	const i32 a = 809;
	const i32 c = 13;
	const i32 m = 1993;

	return (i32)(mathRandomSeed = ((a * mathRandomSeed + c) % m)) % max;
}

f32 randomFloat(f32 low, f32 high)
{
	return (((f32)(rand() & 0x7fff) / (f32) 0x7fff) * (high - low) + low);
}

f32 randomNormalizedFloat()
{
	return (f32) rand() / (f32) RAND_MAX;
}

Vec3 closestPointOnSegment(
	const Vec3& segmentPtA,
	const Vec3& segmentPtB,
	const Vec3& fromPoint,
	bool& outIsOnEdge)
{
	Vec3 c = fromPoint - segmentPtA;
	Vec3 v = segmentPtB - segmentPtA;

	v.normalize();

	outIsOnEdge = false;

	f32 t = v.dot(c);

	if (t < 0.0f)
		return segmentPtA;

	f32 d = segmentPtB.getDistance(segmentPtA);

	if (t > d)
		return segmentPtB;

	outIsOnEdge = true;

	return segmentPtA + v * t;
}

Vec3 closestPointOnLine(const Vec3& linePtA, const Vec3& linePtB, const Vec3& fromPoint)
{
	Vec3 c = fromPoint - linePtA;
	Vec3 v = linePtB - linePtA;

	v.normalize();
	f32 ti = v.dot(c);
	f32 d = linePtB.getDistance(linePtA);

	return  linePtA + v * ti;
}

bool isPointInsideEdge(const Vec3& linePtA, const Vec3& linePtB, const Vec3& point)
{
	return (Vec3(linePtA - point).dot(Vec3(linePtB - point)) <= 0.0f);
}

void projectPoints(
	const Vec3* inPoints,
	u32 count,
	Vec3* outPoints,
	const Rect& viewport,
	const Matrix& world,
	const Matrix& view,
	const Matrix& projection)
{
	Matrix m1, m2;
	Vec3 v;

	m1 = world * view;
	m2 = m1 * projection;

	for (u32 i = 0 ; i < count; ++i)
	{
		m2.transform(inPoints[i], v);
		outPoints[i].x = viewport.x + (1.0f + v.x) * viewport.width / 2.0f;
		outPoints[i].y = viewport.y + (1.0f - v.y) * viewport.height / 2.0f;
		outPoints[i].z = v.z;
	}
}

void unProjectPoints(
	const Vec3* inPoints,
	u32 count,
	Vec3* outPoints,
	const Rect& viewport,
	const Matrix& world,
	const Matrix& view,
	const Matrix& projection)
{
	Matrix m1, m2;

	m1 = world * view;
	m2 = m1 * projection;
	m2.invert();

	for (u32 i = 0 ; i < count; ++i)
	{
		outPoints[i].x = 2.0f * (inPoints[i].x - viewport.x) / viewport.width - 1.0f;
		outPoints[i].y = 1.0f - 2.0f * (inPoints[i].y - viewport.y) / viewport.height;
		outPoints[i].z = inPoints[i].z;
		m2.transform(outPoints[i], outPoints[i]);
	}
}

Ray computePickRay(
	i32 screenX,
	i32 screenY,
	const Rect& viewport,
	const Matrix& world,
	const Matrix& view,
	const Matrix& projection)
{
	Ray ray;
	Vec3 newVec;
	Vec3 v, vPickRayDir, vPickRayOrig;
	Matrix matProj, matView, matWorld;

	matView = view;
	matProj = projection;
	matWorld = world;

	matView.invert();
	
	v.x = -(((f32)(2.0f * (viewport.width - screenX)) / viewport.width) - 1.0f) / matProj.m[0][0];
	v.y = (((f32)(2.0f * (viewport.height - screenY)) / viewport.height) - 1.0f) / matProj.m[1][1];
	v.z = 1.0f;

	vPickRayDir.x  = v.x * matView.m[0][0] + v.y * matView.m[1][0] + v.z * matView.m[2][0];
	vPickRayDir.y  = v.x * matView.m[0][1] + v.y * matView.m[1][1] + v.z * matView.m[2][1];
	vPickRayDir.z  = v.x * matView.m[0][2] + v.y * matView.m[1][2] + v.z * matView.m[2][2];
	
	vPickRayDir.normalize();

	vPickRayOrig.x = matView.m[3][0];
	vPickRayOrig.y = matView.m[3][1];
	vPickRayOrig.z = matView.m[3][2];
	
	vPickRayOrig += vPickRayDir * 0.1f;
	vPickRayDir *= 1000000.0f;
	ray.setStart(Vec3(vPickRayOrig.x, vPickRayOrig.y, vPickRayOrig.z));
	ray.setDirection(Vec3(vPickRayDir.x, vPickRayDir.y, vPickRayDir.z));

	return ray;
}

}