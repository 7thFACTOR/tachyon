// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include <math.h>
#include "base/math/intersection.h"
#include "base/math/util.h"
#include "base/math/vec3.h"
#include "base/math/ray.h"
#include "base/math/bbox.h"
#include "base/math/oriented_bbox.h"
#include "base/math/plane.h"

namespace base
{
const f64 mathEpsilon = 0.00001f;

bool intersectSphereSphere(
	const Vec3& sphereCenter1, const Vec3& sphereCenter2,
	const Vec3& velocity1, const Vec3& velocity2,
	f32 radius1, f32 radius2,
	f32& outTimeIn, f32& outTimeOut)
{
	Vec3 vab(velocity2 - velocity1);
	Vec3 ab(sphereCenter2 - sphereCenter1);
	f32 rab = radius1 + radius2;

	if (ab.dot(ab) <= (rab * rab))
	{
		outTimeIn = 0.0f;
		outTimeOut = 0.0f;
		return true;
	}
	else
	{
		f32 a = vab.dot(vab);

		if ((a < -mathEpsilon) || (a > mathEpsilon))
		{
			f32 b = vab.dot(ab) * 2.0f;
			f32 c = ab.dot(ab) - (rab * rab);
			f32 q = b * b - 4.0f * a * c;

			if (q >= 0.0f)
			{
				f32 sq = (f32) sqrtf(q);
				f32 d  = 1.0f / (2.0f * a);
				f32 r1 = (-b + sq) * d;
				f32 r2 = (-b - sq) * d;

				if (r1 < r2)
				{
					outTimeIn = r1;
					outTimeOut = r2;
				}
				else
				{
					outTimeIn = r2;
					outTimeOut = r1;
				}

				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
}

bool intersectRaySphere(
	const Vec3& center,
	f32 radius,
	const Ray& ray,
	f32& outHitTime,
	Vec3& outHitPosIn,
	Vec3& outHitPosOut)
{
	Vec3 kDiff = ray.getStartPoint() - center;
	f32 fA = ray.getDirection().getSquaredLength();
	f32 fB = kDiff.dot(ray.getDirection());
	f32 fC = kDiff.getSquaredLength() - radius * radius;

	f32 afT[2];
	f32 fDiscr = fB * fB - fA * fC;
	f32 riQuantity;

	if (fDiscr < 0.0f)
	{
		riQuantity = 0.0f;
		return false;
	}
	else if (fDiscr > 0.0f)
	{
		f32 fRoot = sqrtf(fDiscr);
		f32 fInvA = 1.0f / fA;

		afT[0] = (-fB - fRoot) * fInvA;
		afT[1] = (-fB + fRoot) * fInvA;

		if (afT[0] >= 0.0f)
		{
			riQuantity = 2.0f;
			outHitTime = afT[0];
			outHitPosIn = ray.getStartPoint() + ray.getDirection() * afT[0];
			outHitPosOut = ray.getStartPoint() + ray.getDirection() * afT[1];

			return true;
		}
		else if (afT[1] >= 0.0f)
		{
			riQuantity = 1.0f;
			outHitTime = afT[1];
			outHitPosIn = ray.getStartPoint() + ray.getDirection() * afT[1];

			return true;
		}
		else
		{
			riQuantity = 0.0f;

			return false;
		}
	}
	else
	{
		afT[0] = -fB / fA;

		if (afT[0] >= 0.0f)
		{
			riQuantity = 1.0f;
			outHitTime = afT[0];
			outHitPosIn = ray.getStartPoint() + ray.getDirection() * afT[0];

			return true;
		}
		else
		{
			riQuantity = 0.0f;

			return false;
		}
	}
}

bool intersectTriangleLine(
	const Vec3& p1,
	const Vec3& p2,
	const Vec3& p3,
	const Vec3& l1,
	const Vec3& l2,
	Vec3& outIntersection,
	f32& outTime,
	bool acceptBackface)
{
	Vec3 edge1, edge2, tvec, pvec, qvec;
	double det, inv_det;
	double t, u, v;
	Vec3 origin = l1;
	Vec3 dir = l2 - l1;

	edge1 = p2 - p1;
	edge2 = p3 - p1;

	pvec = dir.cross(edge2);
	det = edge1.dot(pvec);

	if (det > -mathEpsilon && det < mathEpsilon)
		return false;

	inv_det = 1.0f / det;
	tvec = origin - p1;
	u = tvec.dot(pvec) * inv_det;

	if (u < 0.0f || u > 1.0f)
		return false;

	qvec = tvec.cross(edge1);
	v = dir.dot(qvec) * inv_det;

	if (v < 0.0f || u + v > 1.0f)
		return false;

	outTime = edge2.dot(qvec) * inv_det;
	outIntersection = B_LERP(l1, l2, outTime);

	if (!acceptBackface)
	{
		Vec3 v1 = edge1.cross(edge2).getNormalized();
		Vec3 v2 = l1 - outIntersection;
		f32 angle = v1.dot(v2);

		if (angle <= 0.0f)
		{
			return false;
		}
	}

	return true;
}

bool intersectLineLine(
	const Vec3& line1A,
	const Vec3& line1B,
	const Vec3& line2A,
	const Vec3& line2B,
	Vec3& outLineA,
	Vec3& outLineB,
	f32& outLineT1,
	f32& outLineT2,
	f32 tolerance)
{
	Vec3 p13, p43, p21;
	double d1343, d4321, d1321, d4343, d2121;
	double numer, denom;

	outLineT1 = 0.0f;
	outLineT2 = 0.0f;

	p13.x = line1A.x - line2A.x;
	p13.y = line1A.y - line2A.y;
	p13.z = line1A.z - line2A.z;
	p43.x = line2B.x - line2A.x;
	p43.y = line2B.y - line2A.y;
	p43.z = line2B.z - line2A.z;

	if (fabs(p43.x) < tolerance
		&& fabs(p43.y) < tolerance
		&& fabs(p43.z) < tolerance)
		return false;

	p21.x = line1B.x - line1A.x;
	p21.y = line1B.y - line1A.y;
	p21.z = line1B.z - line1A.z;

	if (fabs(p21.x) < tolerance
		&& fabs(p21.y) < tolerance
		&& fabs(p21.z) < tolerance)
		return false;

	d1343 = p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
	d4321 = p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
	d1321 = p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
	d4343 = p43.x * p43.x + p43.y * p43.y + p43.z * p43.z;
	d2121 = p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;

	denom = d2121 * d4343 - d4321 * d4321;

	if (fabs(denom) < tolerance)
		return false;

	numer = d1343 * d4321 - d1321 * d4343;

	outLineT1 = numer / denom;
	outLineT2 = (d1343 + d4321 * outLineT1) / d4343;

	outLineA.x = line1A.x + outLineT1 * p21.x;
	outLineA.y = line1A.y + outLineT1 * p21.y;

	outLineA.z = line1A.z + outLineT1 * p21.z;
	outLineB.x = line2A.x + outLineT2 * p43.x;

	outLineB.y = line2A.y + outLineT2 * p43.y;
	outLineB.z = line2A.z + outLineT2 * p43.z;

	return true;
}

bool intersectSegmentSegment(
	const Vec3& line1A,
	const Vec3& line1B,
	const Vec3& line2A,
	const Vec3& line2B,
	Vec3& outLineA,
	Vec3& outLineB,
	f32& outLineT1,
	f32& outLineT2,
	f32 smallestDistance,
	f32 tolerance)
{
	bool bIntersect = intersectLineLine(
		line1A, line1B, line2A, line2B,
		outLineA, outLineB,
		outLineT1, outLineT2,
		tolerance);

	if (bIntersect)
	{
		if (outLineT1 >= 0.0f
			&& outLineT2 >= 0.0f
			&& outLineT1 <= 1.0f
			&& outLineT2 <= 1.0f
			&& outLineA.getDistance(outLineB) <= smallestDistance)
		{
			return true;
		}
	}

	return false;
}

bool intersectLinePolyline(
	const Vec3& lineA,
	const Vec3& lineB,
	const Array<Vec3>& polyline,
	f32 smallestDistance,
	f32 tolerance)
{
	Vec3 a, b;
	f32 t1, t2;

	if (polyline.size() <= 1)
	{
		return false;
	}

	for (u32 i = 0, iCount = polyline.size() - 1; i < iCount; ++i)
	{
		if (intersectSegmentSegment(
					lineA, lineB,
					polyline[i], polyline[i + 1],
					a, b, t1, t2,
					smallestDistance, tolerance))
		{
			return true;
		}
	}

	return false;
}

bool intersectPlaneRay(const Plane& plane, const Ray& ray, f32& outTime)
{
	Vec3 abc(plane.a, plane.b, plane.c);

	f32 dot = abc.dot(ray.getDirection());

	if (dot < 0.0f)
	{
		outTime = -(abc.dot(ray.getStartPoint()) + plane.d) / dot;
		return true;
	}
	else
	{
		return false;
	}
}

bool intersectPlanePlane(const Plane& p1, const Plane& p2, Ray& outRay)
{
	Vec3 n0 = p1.getNormal();
	Vec3 n1 = p2.getNormal();
	f32 n00 = n0.dot(n0);
	f32 n01 = n0.dot(n1);
	f32 n11 = n1.dot(n1);
	f32 det = n00 * n11 - n01 * n01;
	const f32 tol = 1e-06f;

	if (fabsf(det) < tol)
	{
		return false;
	}
	else
	{
		if (det < 0.0001f)
		{
			return false;
		}

		f32 inv_det = 1.0f / det;
		f32 c0 = (n11 * p1.d - n01 * p2.d) * inv_det;
		f32 c1 = (n00 * p2.d - n01 * p1.d) * inv_det;
		outRay.setDirection(n0 * n1);
		outRay.setStart(n0 * c0 + n1 * c1);

		return true;
	}
}

static BBox::ClipType getBoxLineTest(f32 v0, f32 v1, f32 w0, f32 w1)
{
	if ((v1 < w0) || (v0 > w1))
		return  BBox::ClipType::Outside;
	else if ((v0 == w0) && (v1 == w1))
		return BBox::ClipType::IsEqual;
	else if ((v0 >= w0) && (v1 <= w1))
		return BBox::ClipType::IsContained;
	else if ((v0 <= w0) && (v1 >= w1))
		return BBox::ClipType::Contains;
	else
		return BBox::ClipType::Clips;
}

BBox::ClipType intersectBoxBox(const BBox& box1, const BBox& box2)
{
	u32 andCode = 0xFFFF;
	u32 orCode = 0;
	u32 cx, cy, cz;

	cx = (u32)getBoxLineTest(box1.getMinCorner().x, box1.getMaxCorner().x, box2.getMinCorner().x, box2.getMaxCorner().x);
	andCode &= cx;
	orCode |= cx;

	cy = (u32)getBoxLineTest(box1.getMinCorner().y, box1.getMaxCorner().y, box2.getMinCorner().y, box2.getMaxCorner().y);
	andCode &= cy;
	orCode |= cy;

	cz = (u32)getBoxLineTest(box1.getMinCorner().z, box1.getMaxCorner().z, box2.getMinCorner().z, box2.getMaxCorner().z);
	andCode &= cz;
	orCode |= cz;

	if (orCode == 0)
	{
		return BBox::ClipType::Outside;
	}
	else if (andCode != 0)
	{
		return (BBox::ClipType)andCode;
	}
	else
	{
		if (cx && cy && cz)
			return BBox::ClipType::Clips;
		else
			return BBox::ClipType::Outside;
	}
}

bool intersectBoxRay(
	const BBox& box,
	const Ray& ray,
	Vec3& outPoint,
	Vec3& outNormal,
	f32& outTime)
{
	f32 tmin, tmax, tymin, tymax, tzmin, tzmax;
	f32 t0 = 0, t1 = 1.0f;
	f32 divx;

	if (ray.getDirection().x >= 0.0f)
	{
		divx = 1.0f / ray.getDirection().x;
		tmin = (box.getMinCorner().x - ray.getStartPoint().x) / ray.getDirection().x;
		tmax = (box.getMaxCorner().x - ray.getStartPoint().x) / ray.getDirection().x;
	}
	else
	{
		tmin = (box.getMaxCorner().x - ray.getStartPoint().x) / ray.getDirection().x;
		tmax = (box.getMinCorner().x - ray.getStartPoint().x) / ray.getDirection().x;
	}

	if (ray.getDirection().y >= 0.0f)
	{
		tymin = (box.getMinCorner().y - ray.getStartPoint().y) / ray.getDirection().y;
		tymax = (box.getMaxCorner().y - ray.getStartPoint().y) / ray.getDirection().y;
	}
	else
	{
		tymin = (box.getMaxCorner().y - ray.getStartPoint().y) / ray.getDirection().y;
		tymax = (box.getMinCorner().y - ray.getStartPoint().y) / ray.getDirection().y;
	}

	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;
	
	if (tymax < tmax)
		tmax = tymax;

	if (ray.getDirection().z >= 0.0f)
	{
		tzmin = (box.getMinCorner().z - ray.getStartPoint().z) / ray.getDirection().z;
		tzmax = (box.getMaxCorner().z - ray.getStartPoint().z) / ray.getDirection().z;
	}
	else
	{
		tzmin = (box.getMaxCorner().z - ray.getStartPoint().z) / ray.getDirection().z;
		tzmax = (box.getMinCorner().z - ray.getStartPoint().z) / ray.getDirection().z;
	}

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;

	if (tzmin > tmin)
		tmin = tzmin;

	if (tzmax < tmax)
		tmax = tzmax;

	outTime = tmin;
	outPoint = ray.getPointAtTime(outTime);

	return ((tmin < t1) && (tmax > t0));
}

//---

bool intersectOrientedBoxRay(
	const OrientedBBox& box,
	const Ray& ray,
	Vec3& outPoint,
	Vec3& outNormal,
	f32& outTime)
{
	f32 tmin, tmax, tymin, tymax, tzmin, tzmax;
	f32 t0 = 0.0f, t1 = 1.0f;
	f32 divx;
	Ray rrRay = ray;
	Vec3 rrPoint, r1, r2;

	r1 = ray.getStartPoint();
	r2 = ray.getEndPoint();
	box.getInverseTransformMatrix().transform(r1, r1);
	box.getInverseTransformMatrix().transform(r2, r2);
	rrRay.set(r1, r2);

	if (rrRay.getDirection().x >= 0.0f)
	{
		divx = 1.0f / rrRay.getDirection().x;
		tmin = (box.getMin().x - rrRay.getStartPoint().x) / rrRay.getDirection().x;
		tmax = (box.getMax().x - rrRay.getStartPoint().x) / rrRay.getDirection().x;
	}
	else
	{
		tmin = (box.getMax().x - rrRay.getStartPoint().x) / rrRay.getDirection().x;
		tmax = (box.getMin().x - rrRay.getStartPoint().x) / rrRay.getDirection().x;
	}

	if (rrRay.getDirection().y >= 0.0f)
	{
		tymin = (box.getMin().y - rrRay.getStartPoint().y) / rrRay.getDirection().y;
		tymax = (box.getMax().y - rrRay.getStartPoint().y) / rrRay.getDirection().y;
	}
	else
	{
		tymin = (box.getMax().y - rrRay.getStartPoint().y) / rrRay.getDirection().y;
		tymax = (box.getMin().y - rrRay.getStartPoint().y) / rrRay.getDirection().y;
	}

	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	if (rrRay.getDirection().z >= 0.0f)
	{
		tzmin = (box.getMin().z - rrRay.getStartPoint().z) / rrRay.getDirection().z;
		tzmax = (box.getMax().z - rrRay.getStartPoint().z) / rrRay.getDirection().z;
	}
	else
	{
		tzmin = (box.getMax().z - rrRay.getStartPoint().z) / rrRay.getDirection().z;
		tzmax = (box.getMin().z - rrRay.getStartPoint().z) / rrRay.getDirection().z;
	}

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;

	if (tzmin > tmin)
		tmin = tzmin;

	if (tzmax < tmax)
		tmax = tzmax;

	outTime = tmin;
	outPoint = rrRay.getPointAtTime(outTime);
	box.getTransformMatrix().transform(outPoint, outPoint);

	return ((tmin < t1) && (tmax > t0));
}

}