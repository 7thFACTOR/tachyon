#include "render/scene/mesh_node.h"
#include "base/math/conversion.h"
#include "core/core.h"
#include "render/renderer.h"
#include "render/resources/mesh_resource.h"
#include "render/resources/texture_resource.h"
#include "render/resources/gpu_program_resource.h"
#include "core/resource_manager.h"
#include "render/scene/scene.h"

namespace engine
{
MeshNode::MeshNode()
{
}

MeshNode::~MeshNode()
{
}

void MeshNode::update(f32 deltaTime)
{
	SceneNode::update(deltaTime);
}

void MeshNode::render()
{}

//void MeshNode::fillRenderPackets()
//{
//	deleteRenderPackets();
//
//	//for (size_t i = 0; i < m_pMeshPart->clusters.size(); ++i)
//	//{
//	//	auto cluster = m_pMeshPart->clusters[i];
//	//	RenderPacket* pRP = new RenderPacket;
//
//	//	pRP->geometryBufferRenderRange.bActive = true;
//	//	pRP->geometryBufferRenderRange.indexCount = cluster->indexCount;
//	//	pRP->geometryBufferRenderRange.indexOffset = cluster->indexOffset;
//	//	pRP->geometryBufferRenderRange.baseVertexIndex = 0;
//	//	pRP->geometryBufferRenderRange.vertexCount = cluster->primitiveCount / 3;
//	//	pRP->pGeometryBuffer = cluster->pGeometryBuffer;
//	//	pRP->pGpuProgram = resources().load<GpuProgram>("gpu_programs/default");
//	//	pRP->pGpuProgramRenderMethod = pRP->pGpuProgram->currentRenderMethod();
//	//	pRP->textures["diffuseSampler"] = resources().load<Texture>("textures/default");
//	//	pRP->textures["normalSampler"] = resources().load<Texture>("textures/default_n");
//	//	pRP->textures["specularSampler"] = resources().load<Texture>("textures/default");
//	//	pRP->pNode = this;
//
//	//	m_renderPackets.append(pRP);
//	//}
//}

}