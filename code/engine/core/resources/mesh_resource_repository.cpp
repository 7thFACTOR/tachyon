#include "base/defines.h"
#include "base/assert.h"
#include "base/logger.h"
#include "core/resources/mesh_resource_repository.h"

namespace engine
{
bool MeshResourceRepository::load(Stream& stream, ResourceId resId)
{
	MeshResource& mesh = *resources[resId];
	u32 indexCount = 0, elementCount = 0, jointCount = 0;

	stream >> mesh.flags;
	stream >> mesh.primitiveType;
	stream >> indexCount;
	stream >> mesh.vertexCount;
	stream >> mesh.texCoordSetCount;
	stream >> elementCount;
	stream >> jointCount;
	stream >> mesh.pivot;
	stream >> mesh.boundingBox.min;
	stream >> mesh.boundingBox.max;
	stream >> mesh.bindShapeMatrix;

	mesh.sizeOfVertex = 0;
	mesh.joints.resize(jointCount);
	mesh.elements.resize(elementCount);
	mesh.indices.resize(indexCount);

	for (u32 i = 0; i < elementCount; ++i)
	{
		auto& element = mesh.elements[i];
		stream >> element.indexOffset;
		stream >> element.indexCount;
		stream >> element.primitiveCount;
	}

	// load vertex data
	if (!!(mesh.flags & MeshResourceFlags::HasPositions))
	{
		mesh.sizeOfVertex += 3 * sizeof(f32);
	}

	if (!!(mesh.flags & MeshResourceFlags::HasNormals))
	{
		mesh.sizeOfVertex += 3 * sizeof(f32);
	}

	if (!!(mesh.flags & MeshResourceFlags::HasBinormals))
	{
		mesh.sizeOfVertex += 3 * sizeof(f32);
	}

	if (!!(mesh.flags & MeshResourceFlags::HasTangents))
	{
		mesh.sizeOfVertex += 3 * sizeof(f32);
	}

	if (!!(mesh.flags & MeshResourceFlags::HasColors))
	{
		mesh.sizeOfVertex += 4 * sizeof(f32);
	}

	if (mesh.texCoordSetCount)
	{
		mesh.sizeOfVertex += mesh.texCoordSetCount * (sizeof(f32) * 2);
	}

	if (jointCount)
	{
		mesh.sizeOfVertex += meshJointsPerVertex * (sizeof(f32) + sizeof(u16));
	}

	mesh.vertices.resize(mesh.vertexCount * mesh.sizeOfVertex);

	if (jointCount)
		stream.read(mesh.joints.data(), jointCount * sizeof(MeshJoint));
	
	if (mesh.vertexCount)
		stream.read(mesh.vertices.data(), mesh.vertexCount * mesh.sizeOfVertex);
	
	if (indexCount)
		stream.read(mesh.indices.data(), indexCount * sizeof(VertexIndexType));

	return true;
}

void MeshResourceRepository::unload(ResourceId resId)
{
	MeshResource& mesh = *resources[resId];

	mesh.elements.clear();
	mesh.joints.clear();
	mesh.indices.clear();
	mesh.vertices.clear();
}

}