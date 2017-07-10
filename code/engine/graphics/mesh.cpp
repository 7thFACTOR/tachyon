#include "graphics/mesh.h"
#include "graphics/gpu_buffer.h"
#include "graphics/graphics.h"
#include "base/math/util.h"
#include "base/assert.h"
#include "base/logger.h"
#include "core/globals.h"
#include "base/assert.h"
#include "core/resources/mesh_resource.h"
#include "core/resource_repository.h"

namespace engine
{
void Mesh::createBuffers()
{
	vertexBuffer = getGraphics().createGpuBuffer();
	B_ASSERT(vertexBuffer);
	vertexBuffer->type = GpuBufferType::Vertices;
	
	indexBuffer = getGraphics().createGpuBuffer();
	B_ASSERT(indexBuffer);
	indexBuffer->type = GpuBufferType::Indices;
}

void Mesh::freeBuffers()
{
	delete vertexBuffer;
	delete indexBuffer;
}

void Mesh::setFromResource(ResourceId newResourceId)
{
	resourceId = newResourceId;
	MeshResource* mesh = getResources().map<MeshResource>(resourceId);

	B_ASSERT(mesh);

	if (!mesh)
	{
		return;
	}

	createBuffers();

	GpuBufferDescriptor desc;

	if (!!(mesh->flags & MeshResourceFlags::HasPositions))
	{
		desc.addPositionElement();
	}

	if (!!(mesh->flags & MeshResourceFlags::HasNormals))
	{
		desc.addNormalElement();
	}

	if (!!(mesh->flags & MeshResourceFlags::HasBinormals))
	{
		desc.addBitangentElement();
	}

	if (!!(mesh->flags & MeshResourceFlags::HasTangents))
	{
		desc.addTangentElement();
	}

	if (!!(mesh->flags & MeshResourceFlags::HasColors))
	{
		desc.addColorElement();
	}

	for (u32 t = 0; t < mesh->texCoordSetCount; ++t)
	{
		desc.addTexCoordElement(t);
	}

	if (mesh->joints.size())
	{
		for (u32 w = 0; w < meshJointsPerVertex; ++w)
		{
			desc.addJointIndexElement(w);
			desc.addJointWeightElement(w);
		}
	}

	B_LOG_DEBUG("Mesh vcount: " << mesh->vertexCount << " icount: " << (u64)mesh->indices.size());

	vertexBuffer->setupElements(desc, mesh->vertexCount);
	vertexBuffer->initialize(mesh->vertices.size(), mesh->vertices.data());
	desc.clear();
	desc.addIndexElement();
	indexBuffer->setupElements(desc, mesh->indices.size());
	indexBuffer->initialize(mesh->indices.size() * sizeof(VertexIndexType), (u8*)mesh->indices.data());
	indexBuffer->primitiveType = mesh->primitiveType;
	getResources().unmap(resourceId);
}

}