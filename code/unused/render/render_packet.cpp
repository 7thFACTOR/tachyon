#include "render/render_packet.h"
#include "render/gpu_program_render_method.h"
#include "game/components/transform.h"

namespace engine
{
RenderPacket::RenderPacket()
{
	pNode = nullptr;
	pGeometryBuffer = nullptr;
	pInstanceBuffer = nullptr;
	gpuProgram = kInvalidResourceId;
	gpuProgramRenderMethodIndex = 0;
	flags = RenderPacketFlags::Visible;
	material = kInvalidResourceId;
	renderPriority = RenderPriorities::Opaque;
	distanceToCamera = 0.0f;
}

}