#pragma once
#include "base/map.h"
#include "base/string.h"
#include "base/defines.h"
#include "base/types.h"
#include "render/geometry_buffer.h"
#include "render/geometry_buffer_range.h"
#include "render/types.h"

namespace engine
{
class SceneNode;
class GeometryBuffer;
class GpuProgramRenderMethod;

//! How to render the packet
namespace RenderPacketFlags
{
	enum RenderPacketFlag
	{
		//! this render packet is visible
		Visible = B_BIT(0),
		//! this render packet will be rendered before all the normal packets are rendered
		BeforeSceneRender = B_BIT(2),
		//! this render packet will be rendered after all the normal packets are rendered
		AfterSceneRender = B_BIT(3)
	};
}
typedef RenderPacketFlags::RenderPacketFlag RenderPacketFlag;

//! A render packet is the rendering atom which holds a vertex buffer pointer
//! and settings on how to render that vertex buffer: transform, gpu program, gpu program preset, textures
struct E_API RenderPacket
{
	RenderPacket();

	SceneNode* pNode;
	//! the geometry buffer to be rendered for this packet
	GeometryBuffer* pGeometryBuffer;
	//! what range of the vertex buffer to render, valid if m_geometryBufferRenderRange.m_bActive
	GeometryBufferRenderRange geometryBufferRenderRange;
	//! if there is an associated instance buffer for the vertex buffer
	VertexBuffer* pInstanceBuffer;
	ResourceId gpuProgram;
	size_t gpuProgramRenderMethodIndex;
	//! the textures to be set
	Map<String, ResourceId> textures;
	//! some additional flags, see RenderPacket::RenderPacketFlags
	u32 flags;
	//! the material used to initialize this packet, if one is set
	ResourceId material;
	RenderPriority renderPriority;
	f32 distanceToCamera;
};

}