#pragma once
#include "base/array.h"
#include "base/string.h"
#include "base/defines.h"
#include "core/defines.h"
#include "base/types.h"
#include "base/math/quat.h"
#include "base/math/vec3.h"
#include "base/math/bbox.h"

namespace engine
{
using namespace base;
class Animation;
class Mesh;
class Material;
class GeometryBuffer;

//! A mesh part
struct MeshPart
{
	MeshPart()
		: pParent(nullptr)
		, pHull(nullptr)
	{
	}

	String name;
	MeshPart* pParent;
	Vec3 translation;
	Vec3 scale;
	Quat rotation;
	BBox localBoundingBox;
	Array<MeshCluster*> clusters;
	GeometryBuffer* pHull;
	Array<MeshPart*> children;
};
}