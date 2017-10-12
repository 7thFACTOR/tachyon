// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "graphics/frustum.h"
#include "base/math/matrix.h"
#include "base/math/bbox.h"

namespace engine
{
using namespace base;

enum class FrustumSide
{
	Right,
	Left,
	Bottom,
	Top,
	Back,
	Front
};

enum class FrustumPlaneDataType
{
	A,
	B,
	C,
	D
};

Frustum::Frustum()
{}

Frustum::~Frustum()
{}

void normalizePlane(f32 frustum[6][4], FrustumSide side)
{
	const f32 epsilon = 0.00001f;

	f32 magnitude = (f32) sqrtf(
		frustum[(int)side][(int)FrustumPlaneDataType::A] * frustum[(int)side][(int)FrustumPlaneDataType::A]
		+ frustum[(int)side][(int)FrustumPlaneDataType::B] * frustum[(int)side][(int)FrustumPlaneDataType::B]
		+ frustum[(int)side][(int)FrustumPlaneDataType::C] * frustum[(int)side][(int)FrustumPlaneDataType::C]);

	if (magnitude <= epsilon)
	{
		frustum[(int)side][(int)FrustumPlaneDataType::A] = 0;
		frustum[(int)side][(int)FrustumPlaneDataType::B] = 0;
		frustum[(int)side][(int)FrustumPlaneDataType::C] = 0;
		frustum[(int)side][(int)FrustumPlaneDataType::D] = 0;
		
		return;
	}

	frustum[(int)side][(int)FrustumPlaneDataType::A] /= magnitude;
	frustum[(int)side][(int)FrustumPlaneDataType::B] /= magnitude;
	frustum[(int)side][(int)FrustumPlaneDataType::C] /= magnitude;
	frustum[(int)side][(int)FrustumPlaneDataType::D] /= magnitude;
}

void Frustum::calculateFrustum(const Matrix& worldMtx, const Matrix& projMtx)
{
	f32 clip[16];
	const f32 *proj = (f32*)projMtx.m, *modl = (f32*)worldMtx.m;

	clip[0] = modl[0] * proj[0] + modl[1] * proj[4] + modl[2] * proj[ 8] + modl[3] * proj[12];
	clip[1] = modl[0] * proj[1] + modl[1] * proj[5] + modl[2] * proj[ 9] + modl[3] * proj[13];
	clip[2] = modl[0] * proj[2] + modl[1] * proj[6] + modl[2] * proj[10] + modl[3] * proj[14];
	clip[3] = modl[0] * proj[3] + modl[1] * proj[7] + modl[2] * proj[11] + modl[3] * proj[15];

	clip[4] = modl[4] * proj[0] + modl[5] * proj[4] + modl[6] * proj[ 8] + modl[7] * proj[12];
	clip[5] = modl[4] * proj[1] + modl[5] * proj[5] + modl[6] * proj[ 9] + modl[7] * proj[13];
	clip[6] = modl[4] * proj[2] + modl[5] * proj[6] + modl[6] * proj[10] + modl[7] * proj[14];
	clip[7] = modl[4] * proj[3] + modl[5] * proj[7] + modl[6] * proj[11] + modl[7] * proj[15];

	clip[ 8] = modl[8] * proj[0] + modl[9] * proj[4] + modl[10] * proj[ 8] + modl[11] * proj[12];
	clip[ 9] = modl[8] * proj[1] + modl[9] * proj[5] + modl[10] * proj[ 9] + modl[11] * proj[13];
	clip[10] = modl[8] * proj[2] + modl[9] * proj[6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[8] * proj[3] + modl[9] * proj[7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[0] + modl[13] * proj[4] + modl[14] * proj[ 8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[1] + modl[13] * proj[5] + modl[14] * proj[ 9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[2] + modl[13] * proj[6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[3] + modl[13] * proj[7] + modl[14] * proj[11] + modl[15] * proj[15];

	frustum[(int)FrustumSide::Right][(int)FrustumPlaneDataType::A] = clip[3] - clip[0];
	frustum[(int)FrustumSide::Right][(int)FrustumPlaneDataType::B] = clip[7] - clip[4];
	frustum[(int)FrustumSide::Right][(int)FrustumPlaneDataType::C] = clip[11] - clip[8];
	frustum[(int)FrustumSide::Right][(int)FrustumPlaneDataType::D] = clip[15] - clip[12];

	normalizePlane(frustum, FrustumSide::Right);

	frustum[(int)FrustumSide::Left][(int)FrustumPlaneDataType::A] = clip[3] + clip[0];
	frustum[(int)FrustumSide::Left][(int)FrustumPlaneDataType::B] = clip[7] + clip[4];
	frustum[(int)FrustumSide::Left][(int)FrustumPlaneDataType::C] = clip[11] + clip[8];
	frustum[(int)FrustumSide::Left][(int)FrustumPlaneDataType::D] = clip[15] + clip[12];

	normalizePlane(frustum, FrustumSide::Left);

	frustum[(int)FrustumSide::Bottom][(int)FrustumPlaneDataType::A] = clip[3] + clip[1];
	frustum[(int)FrustumSide::Bottom][(int)FrustumPlaneDataType::B] = clip[7] + clip[5];
	frustum[(int)FrustumSide::Bottom][(int)FrustumPlaneDataType::C] = clip[11] + clip[9];
	frustum[(int)FrustumSide::Bottom][(int)FrustumPlaneDataType::D] = clip[15] + clip[13];

	normalizePlane(frustum, FrustumSide::Bottom);

	frustum[(int)FrustumSide::Top][(int)FrustumPlaneDataType::A] = clip[3] - clip[1];
	frustum[(int)FrustumSide::Top][(int)FrustumPlaneDataType::B] = clip[7] - clip[5];
	frustum[(int)FrustumSide::Top][(int)FrustumPlaneDataType::C] = clip[11] - clip[9];
	frustum[(int)FrustumSide::Top][(int)FrustumPlaneDataType::D] = clip[15] - clip[13];

	normalizePlane(frustum, FrustumSide::Top);

	frustum[(int)FrustumSide::Back][(int)FrustumPlaneDataType::A] = clip[3] - clip[2];
	frustum[(int)FrustumSide::Back][(int)FrustumPlaneDataType::B] = clip[7] - clip[6];
	frustum[(int)FrustumSide::Back][(int)FrustumPlaneDataType::C] = clip[11] - clip[10];
	frustum[(int)FrustumSide::Back][(int)FrustumPlaneDataType::D] = clip[15] - clip[14];

	normalizePlane(frustum, FrustumSide::Back);

	frustum[(int)FrustumSide::Front][(int)FrustumPlaneDataType::A] = clip[3] + clip[2];
	frustum[(int)FrustumSide::Front][(int)FrustumPlaneDataType::B] = clip[7] + clip[6];
	frustum[(int)FrustumSide::Front][(int)FrustumPlaneDataType::C] = clip[11] + clip[10];
	frustum[(int)FrustumSide::Front][(int)FrustumPlaneDataType::D] = clip[15] + clip[14];

	normalizePlane(frustum, FrustumSide::Front);
}

bool Frustum::isPointInFrustum(const Vec3& point) const
{
	for (u32 i = 0; i < 6; ++i)
	{
		if (frustum[i][(int)FrustumPlaneDataType::A] * point.x
			+ frustum[i][(int)FrustumPlaneDataType::B] * point.y
			+ frustum[i][(int)FrustumPlaneDataType::C] * point.z
			+ frustum[i][(int)FrustumPlaneDataType::D] <= 0)
		{
			return false;
		}
	}

	return true;
}

bool Frustum::isSphereInFrustum(f32 x, f32 y, f32 z, f32 radius) const
{
	for (u32 i = 0; i < 6; ++i)
	{
		if (frustum[i][(int)FrustumPlaneDataType::A] * x
			+ frustum[i][(int)FrustumPlaneDataType::B] * y
			+ frustum[i][(int)FrustumPlaneDataType::C] * z
			+ frustum[i][(int)FrustumPlaneDataType::D] <= -radius)
		{
			return false;
		}
	}

	return true;
}

bool Frustum::isCubeInFrustum(f32 x, f32 y, f32 z, f32 size) const
{
	u32 hits = 0, oneOut, count = 0;

	for (u32 i = 0; i < 6; ++i)
	{
		hits = 0;
		oneOut = 0;

		if (frustum[i][(int)FrustumPlaneDataType::A] * (x - size)
			+ frustum[i][(int)FrustumPlaneDataType::B] * (y - size)
			+ frustum[i][(int)FrustumPlaneDataType::C] * (z - size)
			+ frustum[i][(int)FrustumPlaneDataType::D] > 0)
		{
			++hits;
		}
		else
		{
			oneOut = 1;
		}

		if (frustum[i][(int)FrustumPlaneDataType::A] * (x + size)
			+ frustum[i][(int)FrustumPlaneDataType::B] * (y - size)
			+ frustum[i][(int)FrustumPlaneDataType::C] * (z - size)
			+ frustum[i][(int)FrustumPlaneDataType::D] > 0)
		{
			++hits;
		}
		else
		{
			oneOut = 1;
		}

		if (frustum[i][(int)FrustumPlaneDataType::A] * (x - size)
			+ frustum[i][(int)FrustumPlaneDataType::B] * (y + size)
			+ frustum[i][(int)FrustumPlaneDataType::C] * (z - size)
			+ frustum[i][(int)FrustumPlaneDataType::D] > 0)
		{
			++hits;
		}
		else
		{
			oneOut = 1;
		}

		if (frustum[i][(int)FrustumPlaneDataType::A] * (x + size)
			+ frustum[i][(int)FrustumPlaneDataType::B] * (y + size)
			+ frustum[i][(int)FrustumPlaneDataType::C] * (z - size)
			+ frustum[i][(int)FrustumPlaneDataType::D] > 0)
		{
			++hits;
		}
		else
		{
			oneOut = 1;
		}

		if (frustum[i][(int)FrustumPlaneDataType::A] * (x - size)
			+ frustum[i][(int)FrustumPlaneDataType::B] * (y - size)
			+ frustum[i][(int)FrustumPlaneDataType::C] * (z + size)
			+ frustum[i][(int)FrustumPlaneDataType::D] > 0)
		{
			++hits;
		}
		else
		{
			oneOut = 1;
		}

		if (frustum[i][(int)FrustumPlaneDataType::A] * (x + size)
			+ frustum[i][(int)FrustumPlaneDataType::B] * (y - size)
			+ frustum[i][(int)FrustumPlaneDataType::C] * (z + size)
			+ frustum[i][(int)FrustumPlaneDataType::D] > 0)
		{
			++hits;
		}
		else
		{
			oneOut = 1;
		}

		if (frustum[i][(int)FrustumPlaneDataType::A] * (x - size)
			+ frustum[i][(int)FrustumPlaneDataType::B] * (y + size)
			+ frustum[i][(int)FrustumPlaneDataType::C] * (z + size)
			+ frustum[i][(int)FrustumPlaneDataType::D] > 0)
		{
			++hits;
		}
		else
		{
			oneOut = 1;
		}

		if (frustum[i][(int)FrustumPlaneDataType::A] * (x + size)
			+ frustum[i][(int)FrustumPlaneDataType::B] * (y + size)
			+ frustum[i][(int)FrustumPlaneDataType::C] * (z + size)
			+ frustum[i][(int)FrustumPlaneDataType::D] > 0)
		{
			++hits;
		}
		else
		{
			oneOut = 1;
		}

		if (hits == 0)
		{
			return false;
		}

		count += oneOut;
	}

	return true;
}

bool Frustum::isBoxInFrustum(const BBox& box) const
{
	u32 hits = 0, oneOut, count = 0;

	for (size_t i = 0; i < 6; ++i)
	{
		hits = 0;
		oneOut = 0;

		if (frustum[i][(int)FrustumPlaneDataType::A] * (box.min.x)
			+ frustum[i][(int)FrustumPlaneDataType::B] * (box.min.y)
			+ frustum[i][(int)FrustumPlaneDataType::C] * (box.min.z)
			+ frustum[i][(int)FrustumPlaneDataType::D] > 0)
		{
			++hits;
		}
		else
		{
			oneOut = 1;
		}

		if (frustum[i][(int)FrustumPlaneDataType::A] * (box.max.x)
			+ frustum[i][(int)FrustumPlaneDataType::B] * (box.min.y)
			+ frustum[i][(int)FrustumPlaneDataType::C] * (box.min.z)
			+ frustum[i][(int)FrustumPlaneDataType::D] > 0)
		{
			++hits;
		}
		else
		{
			oneOut = 1;
		}

		if (frustum[i][(int)FrustumPlaneDataType::A] * (box.min.x)
			+ frustum[i][(int)FrustumPlaneDataType::B] * (box.max.y)
			+ frustum[i][(int)FrustumPlaneDataType::C] * (box.min.z)
			+ frustum[i][(int)FrustumPlaneDataType::D] > 0)
		{
			++hits;
		}
		else
		{
			oneOut = 1;
		}

		if (frustum[i][(int)FrustumPlaneDataType::A] * (box.max.x)
			+ frustum[i][(int)FrustumPlaneDataType::B] * (box.max.y)
			+ frustum[i][(int)FrustumPlaneDataType::C] * (box.min.z)
			+ frustum[i][(int)FrustumPlaneDataType::D] > 0)
		{
			++hits;
		}
		else
		{
			oneOut = 1;
		}

		if (frustum[i][(int)FrustumPlaneDataType::A] * (box.min.x)
			+ frustum[i][(int)FrustumPlaneDataType::B] * (box.min.y)
			+ frustum[i][(int)FrustumPlaneDataType::C] * (box.max.z)
			+ frustum[i][(int)FrustumPlaneDataType::D] > 0)
		{
			++hits;
		}
		else
		{
			oneOut = 1;
		}

		if (frustum[i][(int)FrustumPlaneDataType::A] * (box.max.x)
			+ frustum[i][(int)FrustumPlaneDataType::B] * (box.min.y)
			+ frustum[i][(int)FrustumPlaneDataType::C] * (box.max.z)
			+ frustum[i][(int)FrustumPlaneDataType::D] > 0)
		{
			++hits;
		}
		else
		{
			oneOut = 1;
		}

		if (frustum[i][(int)FrustumPlaneDataType::A] * (box.min.x)
			+ frustum[i][(int)FrustumPlaneDataType::B] * (box.max.y)
			+ frustum[i][(int)FrustumPlaneDataType::C] * (box.max.z)
			+ frustum[i][(int)FrustumPlaneDataType::D] > 0)
		{
			++hits;
		}
		else
		{
			oneOut = 1;
		}

		if (frustum[i][(int)FrustumPlaneDataType::A] * (box.max.x)
			+ frustum[i][(int)FrustumPlaneDataType::B] * (box.max.y)
			+ frustum[i][(int)FrustumPlaneDataType::C] * (box.max.z)
			+ frustum[i][(int)FrustumPlaneDataType::D] > 0)
		{
			++hits;
		}
		else
		{
			oneOut = 1;
		}

		if (hits == 0)
		{
			return false;
		}

		count += oneOut;
	}

	return true;
}

}