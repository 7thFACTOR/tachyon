// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/types.h"
#include "base/math/vec3.h"
#include "base/math/matrix.h"
#include "base/math/bbox.h"
#include "core/defines.h"

namespace base
{
class BBox;
}

namespace engine
{
using namespace base;

enum class FrustumCullType
{
	Outside,
	Inside,
	Intersect
};

class E_API Frustum
{
public:
	Frustum();
	virtual ~Frustum();

	void calculateFrustum(const Matrix& worldMtx, const Matrix& projMtx);
	bool isPointInFrustum(const Vec3& point) const;
	bool isSphereInFrustum(f32 x, f32 y, f32 z, f32 radius) const;
	bool isCubeInFrustum(f32 x, f32 y, f32 z, f32 size) const;
	bool isBoxInFrustum(const BBox& box) const;

private:
	//! the frustum planes [0..6] [A][B][C][D] (plane's equation)
	f32 frustum[6][4];
};

}