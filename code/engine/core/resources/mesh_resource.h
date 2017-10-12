// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/array.h"
#include "base/defines.h"
#include "base/types.h"
#include "core/resource.h"
#include "base/math/matrix.h"
#include "base/math/bbox.h"
#include "graphics/types.h"

namespace engine
{
const u32 meshJointsPerVertex = 4;

enum class MeshResourceFlags
{
	None,
	HasPositions = B_BIT(0),
	HasNormals = B_BIT(1),
	HasTangents = B_BIT(2),
	HasBinormals = B_BIT(3),
	HasColors = B_BIT(4)
};
B_ENUM_AS_FLAGS(MeshResourceFlags);

struct MeshElement
{
	u32 indexOffset = 0;
	u32 indexCount = 0;
	u32 primitiveCount = 0;
};

#pragma pack(push, 1)
struct MeshJoint
{
	u32 id = 0;
	u32 parentId = 0;
	Matrix invertedBindMatrix;
	Matrix localMatrix;
};
#pragma pack(pop)

//! The mesh data hold only the actual naked mesh data (no textures or gpu program info)
struct MeshResource : Resource
{
	MeshResourceFlags flags = MeshResourceFlags::None;
	RenderPrimitive primitiveType = RenderPrimitive::Triangles;
	u32 texCoordSetCount = 0;
	u32 sizeOfVertex = 0;
	u32 vertexCount = 0;
	Array<u8> vertices;
	Array<VertexIndexType> indices;
	//! the bind pose shape/mesh matrix, used in skinning
	Matrix bindShapeMatrix;
	Vec3 pivot;
	BBox boundingBox;
	Array<MeshJoint> joints;
	Array<MeshElement> elements;
};

}