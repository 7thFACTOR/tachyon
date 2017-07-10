#include <string.h>
#include "render/scene/scene_renderer.h"
#include "render/scene/scene.h"
#include "render/resources/gpu_program_resource.h"
#include "render/resources/material_resource.h"
#include "core/core.h"
#include "base/assert.h"
#include "base/logger.h"
#include "base/profiler.h"
#include "base/qsort.h"
#include "base/variable.h"
#include "game/components/camera.h"
#include "game/types.h"
#include "game/component.h"
#include "game/game_manager.h"
#include "render/gpu_program_constant.h"
#include "render/renderer.h"
#include "render/scene/shadow_manager.h"
#include "render/postprocess/postprocess_manager.h"
#include "render/renderer.h"
#include "render/aux_renderer.h"
#include "core/resource_manager.h"
#include "base/math/conversion.h"

namespace engine
{
bool sortNodesByRenderPriority(SceneNode** pN1, SceneNode** pN2)
{
	B_ASSERT(*pN1);
	B_ASSERT(*pN2);

	if (!*pN1 || !*pN2)
	{
		return false;
	}

	u32 prio1 = (*pN1)->m_renderLayerIndex;
	u32 prio2 = (*pN2)->m_renderLayerIndex;

	return prio1 < prio2;
}

bool sortNodesByCameraDistanceBackToFront(SceneNode** pN1, SceneNode** pN2)
{
	B_ASSERT(*pN1);
	B_ASSERT(*pN2);

	if (!*pN1 || !*pN2)
	{
		return false;
	}

	f32 dist1 = (*pN1)->m_distanceToCamera;
	f32 dist2 = (*pN2)->m_distanceToCamera;

	return (dist1 < dist2);
}

bool sortNodesByCameraDistanceFrontToBack(SceneNode** pN1, SceneNode** pN2)
{
	B_ASSERT(*pN1);
	B_ASSERT(*pN2);

	if (!*pN1 || !*pN2)
	{
		return false;
	}

	f32 dist1 = (*pN1)->m_distanceToCamera;
	f32 dist2 = (*pN2)->m_distanceToCamera;

	return (dist1 > dist2);
}

SceneRenderer::SceneRenderer()
{
	m_pPostProcessManager = nullptr;
	m_pShadowManager = nullptr;
	m_pPostProcessManager = new PostProcessManager();
	m_pShadowManager = new ShadowManager();
	m_pScene = nullptr;
}

SceneRenderer::~SceneRenderer()
{
	delete m_pPostProcessManager;
	delete m_pShadowManager;
}

void SceneRenderer::update(Scene* pScene)
{
	m_pScene = pScene;
	gatherNodes();

	//B_BEGIN_PROFILE_SECTION(SceneRenderer_sort_packets);

	if (!m_nodes.isEmpty())
	{
		B_QSORT(SceneNode*, m_nodes.data(), m_nodes.size(), sortNodesByRenderPriority);
	}

	//B_END_PROFILE_SECTION(SceneRenderer_sort_packets);
}

void SceneRenderer::gatherNodes()
{
	m_nodes.clear();
	gatherNodesRecursive(m_pScene->root());
}

void SceneRenderer::gatherNodesRecursive(SceneNode* pNode)
{
	m_nodes.append(pNode);

	for (auto node : pNode->m_children)
	{
		gatherNodesRecursive(node);
	}
}

void SceneRenderer::renderNodes(u32 flag)
{
	for (auto node : m_nodes)
	{
		engine::renderer().setWorldMatrix(node->m_worldMatrix);

		//
		// RULE: if a render packet has a Material set, then the material will dominate gpu program, gpu program preset and unset textures
		// if textures are specified for the packet, then they will override the packet's material (if set)
		//
		// choose from where to get the packet gpu program, textures, etc., if material is set, then get from there
		//GpuProgram* pProgram = 
		//	pPacket->pMaterial
		//	? pPacket->pMaterial->layers()[0]->gpuProgram()
		//	: pPacket->pGpuProgram;
		
		//GpuProgramRenderMethod* pRenderMethod =
		//	pPacket->pMaterial 
		//	? pPacket->pMaterial->layers()[0]->gpuProgramRenderMethod()
		//	: pPacket->pGpuProgramRenderMethod;

		// only render packets that have a shader and a vertex buffer assigned
//		if (pProgram && pPacket->pGeometryBuffer)
//		{
//			pProgram->setCurrentRenderMethod(pRenderMethod);
//			pProgram->bind();
//
//			if (pPacket->pMaterial)
//			{
//				//TODO
//			}
//
//			if (!pPacket->textures.isEmpty())
//			{
//				// set all textures from render packet
//				for (auto iter = pPacket->textures.begin(),
//					iterEnd = pPacket->textures.end();
//					iter != iterEnd; ++iter)
//				{
//					pProgram->currentRenderMethod()->setConstantValue<Texture*>(iter->key.c_str(), iter->value);
//				}
//			}
//
//			pProgram->commitConstants();
//
//			// where the magic happens, the very draw of meshes
//			if (pPacket->geometryBufferRenderRange.bActive)
//			{
//				engine::render().renderGeometry(
//						pProgram,
//						pPacket->pGeometryBuffer,
//						pPacket->pInstanceBuffer,
//						pPacket->geometryBufferRenderRange.baseVertexIndex,
//						pPacket->geometryBufferRenderRange.indexOffset,
//						pPacket->geometryBufferRenderRange.vertexCount,
//						pPacket->geometryBufferRenderRange.indexCount);
//			}
//			else
//			{
//				engine::render().renderGeometry(
//						pProgram,
//						pPacket->pGeometryBuffer,
//						pPacket->pInstanceBuffer);
//			}
//
//			pProgram->unbind();
//		}
//#if defined(_DEBUG)
//		else
//		{
//			if (pPacket->pNode)
//			{
//				if (!pProgram)
//					B_LOG_DEBUG("WARNING: no gpu program assigned to render packet of node @%p",
//						pPacket->pNode);
//
//				if (!pPacket->pGeometryBuffer)
//					B_LOG_DEBUG("WARNING: no geometry buffer assigned to render packet of node @%p",
//						pPacket->pNode);
//			}
//		}
//#endif
	}
}

void SceneRenderer::render(Scene* pScene)
{
	engine::renderer().beginRender();
	engine::renderer().clear(ClearBufferFlags::all(), Color::kBlue);
	m_pScene = pScene;
	//B_BEGIN_PROFILE_SECTION(SceneRenderer_render);
	renderNodes(-1);
	renderNodes(0);
	renderNodes(0xffffffff);
	//B_END_PROFILE_SECTION(SceneRenderer_render);

	Matrix proj;
	Matrix world;
	Matrix view;

	proj.perspective(67, 768.0f/1024.0f, 1.0f, 10000.0f);
	Quat q;
	static f32 a = 0;
	toQuat(a+=1.3f, 0, 1,0, q);
	toMatrix(q, world);

	view.setTranslation(0, -1, 2.0f);

	engine::renderer().rasterizerState().cullMode = CullMode::None;
	engine::renderer().depthStencilState().bDepthTest = false;
	engine::renderer().setProjectionMatrix(proj);
	engine::renderer().setWorldMatrix(world);
	engine::renderer().setViewMatrix(view);
	engine::renderer().resetViewport();
	engine::renderer().bindGpuProgram(hashStringSbdm("gpu_programs/default.gpu_program"));

	engine::renderer().depthStencilState().bDepthTest = true;
	engine::renderer().depthStencilState().bDepthWrite =true;
	engine::renderer().rasterizerState().bAlphaTest = false;
	engine::renderer().rasterizerState().cullMode = CullMode::CCW;

	ResourceId id = hashStringSbdm("meshes/tree.mesh");
	resources().load(id);
	auto iter = engine::renderer().geometryBuffers().find(id);
	int o = engine::renderer().geometryBuffers().size();
	if (iter != engine::renderer().geometryBuffers().end())
	{
		auto p = iter->value;
		engine::renderer().renderGeometry(
			hashStringSbdm("gpu_programs/default.gpu_program"),	p);
	}

	auxRenderer()->begin();
	auxRenderer()->set2dProjection();
	ResourceId tex = hashStringSbdm("textures/default.texture");
	resources().load(tex);
	auxRenderer()->draw2dTexturedQuad(tex, 0, 0, 0.1, 0.1);
	auxRenderer()->end();
	
	engine::renderer().endRender();
	engine::renderer().present();
}

ShadowManager* SceneRenderer::shadowManager() const
{
	return m_pShadowManager;
}

}