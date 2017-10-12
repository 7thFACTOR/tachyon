// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "core/resources/material_resource_repository.h"
#include "base/defines.h"
#include "base/assert.h"
#include "base/logger.h"
#include "core/resources/gpu_program_resource.h"
#include "core/globals.h"
#include "core/resource_repository.h"

namespace engine
{
bool MaterialResourceRepository::load(Stream& stream, ResourceId resId)
{
	MaterialResource& mtl = *resources[resId];

	stream >> mtl.gpuProgramId;

	u32 stateCount = 0;
	RenderState stateType;
	f32 fStateValue;
	i32 iStateValue;
	bool isFloat;

	stream >> stateCount;

	for (u32 i = 0; i < stateCount; i++)
	{
		stream >> stateType;
		stream >> isFloat;

		if (isFloat)
		{
			stream >> fStateValue;
		}
		else
		{
			stream >> iStateValue;
		}

		switch (stateType)
		{
		case RenderState::None:
			break;
		case RenderState::AlphaBlendEnable:
			mtl.blendState.renderTarget[0].enable = (bool)iStateValue;
			break;
		case RenderState::AlphaRef:
			mtl.blendState.alphaRef = fStateValue;
			break;
		case RenderState::AlphaTestEnable:
			mtl.rasterizerState.alphaTest = (bool)iStateValue;
			break;
		case RenderState::ColorWriteEnable:
			mtl.blendState.renderTarget[0].renderTargetWriteMask = (bool)iStateValue ? ~0 : 0;
			break;
		case RenderState::DepthBias:
			mtl.rasterizerState.depthBias = fStateValue;
			break;
		case RenderState::DestBlend:
			mtl.blendState.renderTarget[0].destBlend = (BlendType)iStateValue;
			break;
		case RenderState::FillMode:
			mtl.rasterizerState.fillMode = (FillMode)iStateValue;
			break;
		case RenderState::SlopeScaleDepthBias:
			//TODO:
			B_ASSERT_NOT_IMPLEMENTED;
			break;
		case RenderState::SrcBlend:
			mtl.blendState.renderTarget[0].srcBlend = (BlendType)iStateValue;
			break;
		case RenderState::StencilEnable:
			mtl.depthStencilState.stencil = (bool)iStateValue;
			break;
		case RenderState::StencilFail:
			mtl.depthStencilState.backFace.failOp = (StencilOperation)iStateValue;
			mtl.depthStencilState.frontFace.failOp = (StencilOperation)iStateValue;
			break;
		case RenderState::StencilFunc:
			mtl.depthStencilState.backFace.function = (CompareFunction)iStateValue;
			mtl.depthStencilState.frontFace.function = (CompareFunction)iStateValue;
			break;
		case RenderState::StencilMask:
			B_ASSERT_NOT_IMPLEMENTED
			//TODO
			break;
		case RenderState::StencilPass:
			mtl.depthStencilState.backFace.passOp = (StencilOperation)iStateValue;
			mtl.depthStencilState.frontFace.passOp = (StencilOperation)iStateValue;
			break;
		case RenderState::StencilRef:
			mtl.depthStencilState.stencilRef = (u8)iStateValue;
			break;
		case RenderState::StencilWriteMask:
			mtl.depthStencilState.stencilWriteMask = (u8)iStateValue;
			break;
		case RenderState::StencilDepthFail:
			mtl.depthStencilState.backFace.depthFailOp = (StencilOperation)iStateValue;
			mtl.depthStencilState.frontFace.depthFailOp = (StencilOperation)iStateValue;
			break;
		case RenderState::DepthTestEnable:
			mtl.depthStencilState.depthTest = (bool)iStateValue;
			break;
		case RenderState::DepthFunc:
			mtl.depthStencilState.depthFunc = (CompareFunction)iStateValue;
			break;
		case RenderState::DepthWriteEnable:
			mtl.depthStencilState.depthWrite = (bool)iStateValue;
			break;
		case RenderState::CullMode:
			mtl.rasterizerState.cullMode = (CullMode)iStateValue;
			break;
		default:
			B_LOG_ERROR("Unknown render state type " << (u32)stateType);
			break;
		}
	}

	u32 constantCount = 0;
	stream >> constantCount;
	mtl.constants.resize(constantCount);

	for (u32 i = 0; i < constantCount; i++)
	{
		GpuProgramConstantData& cdata = mtl.constants[i];

		stream >> cdata.type;
		stream >> cdata.name;
		stream >> cdata.valueCount;
		stream >> cdata.dataSize;
		//TODO: maybe a mem pool for data?
		cdata.data = new u8[cdata.dataSize];
		B_ASSERT(cdata.data);
		stream.read(cdata.data, cdata.dataSize);

		B_LOG_DEBUG("\tMaterial constant type: " << (u32)cdata.type << " name: " << cdata.name << " valueCount: " << cdata.valueCount << " dataSize: " << cdata.dataSize);

		// start load textures
		if (cdata.type == GpuProgramConstantType::Texture)
		{
			u64* values = (u64*)cdata.data;

			if (cdata.valueCount == 1)
			{
				getResources().load(values[0]);
			}
			else
			{
				// load the textures
				for (size_t t = 0; t < cdata.valueCount; t++)
				{
					getResources().load(values[t]);
				}
			}
		}
	}

	return true;
}

void MaterialResourceRepository::unload(ResourceId resId)
{
	MaterialResource& mtl = *resources[resId];

	mtl.constants.clear();
}

}