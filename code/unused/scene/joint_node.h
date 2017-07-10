#pragma once
#include "base/string.h"
#include "base/defines.h"
#include "base/types.h"
#include "game/components/transform.h"
#include "base/math/vec3.h"
#include "base/math/matrix.h"

namespace engine
{
struct SkeletonJoint
{
	String name;
	String parentName;
	Matrix bindPoseInverseMatrix;
	Matrix offsetMatrix;
	Vec3 minAngleLimits;
	Vec3 maxAngleLimits;
	Vec3 translation;
	Quat rotation;
	bool bLimitX, bLimitY, bLimitZ;
};

}