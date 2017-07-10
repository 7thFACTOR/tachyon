#include <stdlib.h>
#include "resources/material.h"
#include "core/core.h"
#include "base/util.h"
#include "base/logger.h"
#include "base/assert.h"
#include "core/resource_manager.h"
#include "base/file.h"
#include "base/stream.h"
#include "resources/gpu_program.h"
#include "resources/texture.h"
#include "render/render_manager.h"

namespace engine
{
MaterialLayer::MaterialLayer()
{
	m_pGpuProgram = nullptr;
	m_pGpuProgramRenderMethod = nullptr;
}

MaterialLayer::~MaterialLayer()
{
	unloadResources();
}

void MaterialLayer::loadResources()
{
	unloadResources();
	resources().load(m_gpuProgram);

	for (size_t i = 0; i < m_textureSamplerInfo.size(); ++i)
	{
		resources().load(m_textureSamplerInfo[i].texture);
	}
}

void MaterialLayer::unloadResources()
{
	resources().release(m_gpuProgram);
	m_pGpuProgram = nullptr;
	m_pGpuProgramRenderMethod = nullptr;

	for (size_t i = 0; i < m_textureSamplerInfo.size(); ++i)
	{
		resources().release(m_textureSamplerInfo[i].texture);
	}
}

void MaterialLayer::setGpuProgram(ResourceId resId)
{
	resources().release(m_gpuProgram);
	m_gpuProgram = resId;
	resources().load(m_gpuProgram);
}

void MaterialLayer::setGpuProgramRenderMethod(const char* pName)
{
	m_gpuProgramRenderMethod = pName;
}

ResourceId MaterialLayer::gpuProgram() const
{
	return m_gpuProgram;
}

const Array<TextureSamplerInfo>& MaterialLayer::textureSamplerInfo() const
{
	return m_textureSamplerInfo;
}

bool MaterialLayer::load(Stream* pStream)
{
	// read render states
	i16 stateCount = 0;
	i16 constCount = 0;
	i16 samplerCount = 0;
	
	*pStream >> m_name;
	*pStream >> m_gpuProgram;
	*pStream >> m_gpuProgramRenderMethod;
	*pStream >> stateCount;

	B_LOG_DEBUG("Reading %d render states", stateCount);

	// read preset render states
	for (size_t i = 0; i < stateCount; ++i)
	{
		RenderState renderState;
		f32 stateValue;

		pStream->readInt32((int&)renderState.m_type);
		pStream->readFloat(stateValue);

		if (renderState.m_type == RenderState::eType_None)
		{
			B_LOG_ERROR("Unknown render state ID: '%d' value: %f", renderState.m_type, stateValue);
			continue;
		}

		switch (renderState.m_type)
		{
		case RenderState::eType_AlphaBlendEnable:
		case RenderState::eType_AlphaTestEnable:
		case RenderState::eType_ColorWriteEnable:
		case RenderState::eType_DepthTestEnable:
		case RenderState::eType_DepthWriteEnable:
		case RenderState::eType_StencilEnable:
			// values: true,false
			renderState.m_value = (bool)((int)stateValue) == 1;
			break;
		case RenderState::eType_AlphaRef:
			// values: a f32
			renderState.m_value = stateValue;
			break;
		case RenderState::eType_DepthBias:
			// values: a f32
			renderState.m_value = stateValue;
			break;
		case RenderState::eType_DestBlend:
			// values: from s_blendTypeMap
			renderState.m_value = (i32)stateValue;
			break;
		case RenderState::eType_FillMode:
			// values: from s_fillModeMap
			renderState.m_value = (i32)stateValue;
			break;
		case RenderState::eType_SlopeScaleDepthBias:
			// values: a f32
			renderState.m_value = (f32)stateValue;
			break;
		case RenderState::eType_SrcBlend:
			// values: from s_blendTypeMap
			renderState.m_value = (i32)stateValue;
			break;
		case RenderState::eType_StencilFail:
			renderState.m_value = (i32)stateValue;
			break;
		case RenderState::eType_StencilFunc:
			// values: from s_cmpFuncMap
			renderState.m_value = (i32)stateValue;
			break;
		case RenderState::eType_StencilMask:
			// values: from s_cmpFuncMap
			renderState.m_value = (i32)stateValue;
			break;
		case RenderState::eType_StencilPass:
			renderState.m_value = (i32)stateValue;
			break;
		case RenderState::eType_StencilRef:
			renderState.m_value = (i32)stateValue;
			break;
		case RenderState::eType_StencilWriteMask:
			renderState.m_value = (i32)stateValue;
			break;
		case RenderState::eType_StencilDepthFail:
			renderState.m_value = (i32)stateValue;
			break;
		case RenderState::eType_DepthFunc:
			// values: from s_cmpFuncMap
			renderState.m_value = (i32)stateValue;
			break;
		case RenderState::eType_CullMode:
			renderState.m_value = (i32)stateValue;
			break;
		};

		B_LOG_DEBUG("Read render state: '%d'='%f'", renderState.m_type, stateValue);
		m_renderStates.append(renderState);
	}

	// constants
	*pStream >> constCount;

	Map<String, String> constants;
	String constName, constValue;

	B_LOG_DEBUG("Reading %d gpu program constants", constCount);

	for (size_t i = 0; i < constCount; ++i)
	{
		*pStream >> constName;
		*pStream >> constValue;
		constants[constName] = constValue;
	}

	// samplers
	*pStream >> samplerCount;
	B_LOG_DEBUG("Reading %d samplers", samplerCount);
	B_LOG_INDENT;

	for (size_t i = 0; i < samplerCount; ++i)
	{
		TextureSamplerInfo tsi;

		*pStream >> tsi.samplerName;
		*pStream >> tsi.fileName;
		*pStream >> (int&)tsi.addressU;
		*pStream >> (int&)tsi.addressV;
		*pStream >> (int&)tsi.addressW;
		*pStream >> (int&)tsi.filter;
		*pStream >> tsi.minLod;
		*pStream >> tsi.maxLod;
		*pStream >> tsi.mipMapLodBias;

		B_LOG_DEBUG("Read sampler: %s, %s, %d, %d, %d, %d, %f, %f, %f",
			tsi.samplerName.c_str(),
			tsi.fileName.c_str(),
			tsi.addressU,
			tsi.addressV,
			tsi.addressW,
			tsi.filter,
			tsi.minLod,
			tsi.maxLod,
			tsi.mipMapLodBias);
	}
	B_LOG_UNINDENT;

	return true;
}

const Array<RenderState>& MaterialLayer::renderStates() const
{
	return m_renderStates;
}

void MaterialLayer::commitRenderStates()
{
	render().rasterizerState() = m_rasterizerState;
	render().blendState() = m_blendState;
	render().depthStencilState() = m_depthStencilState;
}

void MaterialLayer::updateRenderStateObjects()
{
	// reset
	m_rasterizerState = RasterizerState();
	m_blendState = BlendState();
	m_depthStencilState = DepthStencilState();

	// set
	for (size_t i = 0; i < m_renderStates.size(); ++i)
	{
		const RenderState& state = m_renderStates[i];

		switch (state.m_type)
		{
		case RenderState::eType_AlphaBlendEnable:
			{
				m_blendState.renderTarget[0].bEnable = (bool)state.m_value;
				break;
			}

		case RenderState::eType_AlphaTestEnable:
			{
				m_rasterizerState.bAlphaTest = (bool)state.m_value;
				break;
			}

		case RenderState::eType_ColorWriteEnable:
			{
				m_blendState.renderTarget[0].renderTargetWriteMask = (bool)state.m_value ? 0xf : 0;
				break;
			}

		case RenderState::eType_DepthTestEnable:
			{
				m_depthStencilState.bDepthTest = (bool)state.m_value;
				break;
			}

		case RenderState::eType_DepthWriteEnable:
			{
				m_depthStencilState.bDepthWrite = (bool)state.m_value;
				break;
			}

		case RenderState::eType_StencilEnable:
			{
				m_depthStencilState.bStencil = (bool)state.m_value;
				break;
			}

		case RenderState::eType_SrcBlend:
			{
				m_blendState.renderTarget[0].srcBlend = (EBlend)(int)state.m_value;
				break;
			}

		case RenderState::eType_DestBlend:
			{
				m_blendState.renderTarget[0].destBlend = (EBlend)(int)state.m_value;
				break;
			}

		case RenderState::eType_DepthBias:
			{
				m_rasterizerState.depthBias = state.m_value;
				break;
			}

		case RenderState::eType_AlphaRef:
			{
				m_rasterizerState.alphaTestMinValue = state.m_value;
				break;
			}

		case RenderState::eType_FillMode:
			{
				m_rasterizerState.fillMode = (EFillMode)(int)state.m_value;
				break;
			}

		case RenderState::eType_SlopeScaleDepthBias:
			{
				m_rasterizerState.slopeScaledDepthBias = state.m_value;
				break;
			}

		case RenderState::eType_StencilFail:
			{
				m_depthStencilState.frontFace.failOp = (EStencilOp)(int)state.m_value;
				break;
			}

		case RenderState::eType_StencilDepthFail:
			{
				m_depthStencilState.frontFace.depthFailOp = (EStencilOp)(int)state.m_value;
				break;
			}

		case RenderState::eType_StencilPass:
			{
				m_depthStencilState.frontFace.passOp = (EStencilOp)(int)state.m_value;
				break;
			}

		case RenderState::eType_StencilFunc:
			{
				m_depthStencilState.frontFace.function = (ECmpFunc)(int)state.m_value;
				break;
			}

		case RenderState::eType_StencilMask:
			{
				m_depthStencilState.stencilReadMask = (i8)state.m_value;
				break;
			}

		case RenderState::eType_StencilWriteMask:
			{
				m_depthStencilState.stencilWriteMask = (i8)state.m_value;
				break;
			}

		case RenderState::eType_StencilRef:
			{
				m_depthStencilState.stencilRef = (i8)state.m_value;
				break;
			}

		case RenderState::eType_DepthFunc:
			{
				m_depthStencilState.depthFunc = (ECmpFunc)(int)state.m_value;
				break;
			}

		case RenderState::eType_CullMode:
			{
				m_rasterizerState.cullMode = (ECullMode)(int)state.m_value;
				break;
			}
		}
	}
}

//---

Material::Material()
{
}

Material::~Material()
{
	B_ASSERT(m_layers.isEmpty());
}

bool Material::load(Stream* pStream)
{
	ChunkHeader chunk;
	String signature;

	while (!pStream->eof())
	{
		if (!pStream->readChunkHeader(&chunk))
			break;

		B_LOG_DEBUG("Reading chunk id %d", chunk.id);

		switch (chunk.id)
		{
		// read file header
		case eMaterialChunk_Header:
			{
				if (chunk.version != Material::kFileVersion)
				{
					B_LOG_ERROR("Material version is different from the material, Engine: ver%d File: ver%d",
						Material::kFileVersion, chunk.version);
					return false;
				}
				break;
			}

		case eMaterialChunk_Layers:
			{
				i16 layerCount;
				*pStream >> layerCount;

				B_LOG_DEBUG("Reading %d material layers", layerCount);

				for (i16 i = 0; i < layerCount; ++i)
				{
					MaterialLayer* pLayer = new MaterialLayer();
					B_ASSERT(pLayer);
					pLayer->load(pStream);
					m_layers.append(pLayer);
				}
				break;
			}
		// ignore chunk if unknown ID
		default:
			{
				B_LOG_DEBUG("Ignoring material chunk id: %d", chunk.id);
				pStream->ignoreChunk(&chunk);
			}
		}
	}

	B_LOG_DEBUG("End material load.");
	return true;
}

bool Material::unload()
{
	free();

	return true;
}

const Array<MaterialLayer*>& Material::layers() const
{
	return m_layers;
}

MaterialLayer* Material::findLayer(const char* pName) const
{
	for (size_t i = 0, iCount = m_layers.size(); i < iCount; ++i)
	{
		if (m_layers[i]->name() == pName)
		{
			return m_layers[i];
		}
	}

	return nullptr;
}

void Material::onDependencyUnload(Resource* pDependencyRes)
{
	if (!pDependencyRes)
	{
		return;
	}

	auto iter = m_layers.begin();

	while (iter != m_layers.end())
	{
		if (pDependencyRes->id() == (*iter)->gpuProgram())
		{
			//(*iter)->setGpuProgram(kInvalidResourceId);
		}
		else
		{
			++iter;
		}
	}
}

void Material::free()
{
	for (size_t i = 0, iCount = m_layers.size(); i < iCount; ++i)
	{
		m_layers[i]->unloadResources();
		delete m_layers[i];
	}

	m_layers.clear();
}
}