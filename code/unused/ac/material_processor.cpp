#include "common.h"
#include "material_processor.h"
#include "base/logger.h"
#include "base/platform.h"
#include "base/util.h"
//#include "render/resources/material_resource.h"
#include "base/json.h"

namespace ac
{
using namespace base;
//B_BEGIN_ENUM_STRINGIFY(textureAddress)
//	B_ADD_ENUM_STRINGIFY("wrap", eTextureAddressType_Wrap)
//	B_ADD_ENUM_STRINGIFY("clamp", eTextureAddressType_Clamp)
//B_END_ENUM_STRINGIFY(textureAddress)
//
//B_BEGIN_ENUM_STRINGIFY(textureFilter)
//	B_ADD_ENUM_STRINGIFY("point", eTextureFilterType_Nearest)
//	B_ADD_ENUM_STRINGIFY("linear", eTextureFilterType_Linear)
//	B_ADD_ENUM_STRINGIFY("anisotropic", eTextureFilterType_Anisotropic)
//B_END_ENUM_STRINGIFY(textureFilter)
//
//B_BEGIN_ENUM_STRINGIFY(cmpFunc)
//	B_ADD_ENUM_STRINGIFY("never", eCmpFunc_Never)
//	B_ADD_ENUM_STRINGIFY("less", eCmpFunc_Less)
//	B_ADD_ENUM_STRINGIFY("equal", eCmpFunc_Equal)
//	B_ADD_ENUM_STRINGIFY("lessEqual", eCmpFunc_LessEqual)
//	B_ADD_ENUM_STRINGIFY("greater", eCmpFunc_Greater)
//	B_ADD_ENUM_STRINGIFY("notEqual", eCmpFunc_NotEqual)
//	B_ADD_ENUM_STRINGIFY("greaterEqual", eCmpFunc_GreaterEqual)
//	B_ADD_ENUM_STRINGIFY("always", eCmpFunc_Always)
//B_END_ENUM_STRINGIFY(cmpFunc)
//
//B_BEGIN_ENUM_STRINGIFY(blendType)
//	B_ADD_ENUM_STRINGIFY("zero", eBlend_Zero)
//	B_ADD_ENUM_STRINGIFY("one", eBlend_One)
//	B_ADD_ENUM_STRINGIFY("srcColor", eBlend_SrcColor)
//	B_ADD_ENUM_STRINGIFY("invSrcColor", eBlend_InvSrcColor)
//	B_ADD_ENUM_STRINGIFY("srcAlpha", eBlend_SrcAlpha)
//	B_ADD_ENUM_STRINGIFY("invSrcAlpha", eBlend_InvSrcAlpha)
//	B_ADD_ENUM_STRINGIFY("dstAlpha", eBlend_DstAlpha)
//	B_ADD_ENUM_STRINGIFY("invDstAlpha", eBlend_InvDstAlpha)
//	B_ADD_ENUM_STRINGIFY("dstColor", eBlend_DstColor)
//	B_ADD_ENUM_STRINGIFY("invDstColor", eBlend_InvDstColor)
//	B_ADD_ENUM_STRINGIFY("srcAlphaSat", eBlend_SrcAlphaSat)
//B_END_ENUM_STRINGIFY(blendType)
//
//B_BEGIN_ENUM_STRINGIFY(renderState)
//	B_ADD_ENUM_STRINGIFY("", RenderState::eType_None)
//	B_ADD_ENUM_STRINGIFY("alphaBlendEnable", RenderState::eType_AlphaBlendEnable)
//	B_ADD_ENUM_STRINGIFY("alphaRef", RenderState::eType_AlphaRef)
//	B_ADD_ENUM_STRINGIFY("alphaTestEnable", RenderState::eType_AlphaTestEnable)
//	B_ADD_ENUM_STRINGIFY("colorWriteEnable", RenderState::eType_ColorWriteEnable)
//	B_ADD_ENUM_STRINGIFY("depthBias", RenderState::eType_DepthBias)
//	B_ADD_ENUM_STRINGIFY("destBlend", RenderState::eType_DestBlend)
//	B_ADD_ENUM_STRINGIFY("fillMode", RenderState::eType_FillMode)
//	B_ADD_ENUM_STRINGIFY("slopeScaleDepthBias", RenderState::eType_SlopeScaleDepthBias)
//	B_ADD_ENUM_STRINGIFY("srcBlend", RenderState::eType_SrcBlend)
//	B_ADD_ENUM_STRINGIFY("stencilEnable", RenderState::eType_StencilEnable)
//	B_ADD_ENUM_STRINGIFY("stencilFail", RenderState::eType_StencilFail)
//	B_ADD_ENUM_STRINGIFY("stencilFunc", RenderState::eType_StencilFunc)
//	B_ADD_ENUM_STRINGIFY("stencilMask", RenderState::eType_StencilMask)
//	B_ADD_ENUM_STRINGIFY("stencilPass", RenderState::eType_StencilPass)
//	B_ADD_ENUM_STRINGIFY("stencilRef", RenderState::eType_StencilRef)
//	B_ADD_ENUM_STRINGIFY("stencilWriteMask", RenderState::eType_StencilWriteMask)
//	B_ADD_ENUM_STRINGIFY("stencilDepthFail", RenderState::eType_StencilDepthFail)
//	B_ADD_ENUM_STRINGIFY("depthTestEnable", RenderState::eType_DepthTestEnable)
//	B_ADD_ENUM_STRINGIFY("depthWriteEnable", RenderState::eType_DepthWriteEnable)
//	B_ADD_ENUM_STRINGIFY("depthFunc", RenderState::eType_DepthFunc)
//	B_ADD_ENUM_STRINGIFY("cullMode", RenderState::eType_CullMode)
//B_END_ENUM_STRINGIFY(renderState)
//
//B_BEGIN_ENUM_STRINGIFY(fillMode)
//	B_ADD_ENUM_STRINGIFY("point", eFillMode_Point)
//	B_ADD_ENUM_STRINGIFY("wireframe", eFillMode_Wireframe)
//	B_ADD_ENUM_STRINGIFY("solid", eFillMode_Solid)
//B_END_ENUM_STRINGIFY(fillMode)
//
//B_BEGIN_ENUM_STRINGIFY(stencilOp)
//	B_ADD_ENUM_STRINGIFY("keep", eStencilOp_Keep)
//	B_ADD_ENUM_STRINGIFY("zero", eStencilOp_Zero)
//	B_ADD_ENUM_STRINGIFY("replace", eStencilOp_Replace)
//	B_ADD_ENUM_STRINGIFY("incrSat", eStencilOp_IncrSat)
//	B_ADD_ENUM_STRINGIFY("decrSat", eStencilOp_DecrSat)
//	B_ADD_ENUM_STRINGIFY("invert", eStencilOp_Invert)
//	B_ADD_ENUM_STRINGIFY("incr", eStencilOp_Incr)
//	B_ADD_ENUM_STRINGIFY("decr", eStencilOp_Decr)
//B_END_ENUM_STRINGIFY(stencilOp)
//
//B_BEGIN_ENUM_STRINGIFY(cullMode)
//	B_ADD_ENUM_STRINGIFY("none", eCullMode_None)
//	B_ADD_ENUM_STRINGIFY("cw", eCullMode_CW)
//	B_ADD_ENUM_STRINGIFY("ccw", eCullMode_CCW)
//B_END_ENUM_STRINGIFY(cullMode)

MaterialProcessor::MaterialProcessor()
{
}

MaterialProcessor::~MaterialProcessor()
{
}

const char* MaterialProcessor::supportedFileExtensions() const
{
	return ".material";
}

AssetProcessor::EType MaterialProcessor::type() const
{
	return eType_Compiler;
}

bool MaterialProcessor::writeMaterial(FileWriter& file, JsonNode* jsnRoot)
{
	JsonNode::Value jsnLayers = jsnRoot->valueOf("layers");

	if (jsnLayers.asNode() && jsnLayers.asNode()->isArray())
	{
		E_LOG_VERBOSE("Writing %d material layers", jsnLayers.asNode()->arrayValues().size());

		//file.beginChunk(eMaterialChunk_Layers, 1);
		file.writeInt16(jsnLayers.asNode()->arrayValues().size());

		for (size_t l = 0; l < jsnLayers.asNode()->arrayValues().size(); ++l)
		{
			JsonNode* jsnLayer = jsnLayers.asNode()->arrayValues()[l]->asNode();

			if (!jsnLayer)
			{
				return false;
			}

			JsonNode* jsnStates = jsnLayer->valueOf("renderStates").asNode();
			JsonNode* jsnConstants = jsnLayer->valueOf("gpuProgramConstants").asNode();
			JsonNode* jsnSamplers = jsnLayer->valueOf("textureSamplers").asNode();

			file.writeString(jsnLayer->valueOf("name").asString());
			file.writeString(jsnLayer->valueOf("gpuProgram").asString());
			file.writeString(jsnLayer->valueOf("gpuProgramRenderMethod").asString());

			// states count
			i16 stateCount = jsnStates ? jsnStates->nameValuePairs().size() : 0;
			file.writeInt16(stateCount);

			E_LOG_VERBOSE("Writing %d render states", stateCount);

			if (jsnStates)
			{
				for (size_t j = 0; j < jsnStates->nameValuePairs().size(); ++j)
				{
					auto jsnState = jsnStates->nameValuePairs().at(j);
					int stateType = 0;// B_STRING_TO_ENUM(renderState, jsnState.key);
					f32 stateValue = 0.0f;

					file.writeInt32(stateType);

					//switch (stateType)
					//{
					//case RenderState::eType_AlphaBlendEnable:
					//	{
					//		stateValue = jsnState.value.asBool();
					//		break;
					//	}

					//case RenderState::eType_AlphaTestEnable:
					//	{
					//		stateValue = jsnState.value.asBool();
					//		break;
					//	}

					//case RenderState::eType_ColorWriteEnable:
					//	{
					//		stateValue = jsnState.value.asBool();
					//		break;
					//	}

					//case RenderState::eType_DepthTestEnable:
					//	{
					//		stateValue = jsnState.value.asBool();
					//		break;
					//	}

					//case RenderState::eType_DepthWriteEnable:
					//	{
					//		stateValue = jsnState.value.asBool();
					//		break;
					//	}

					//case RenderState::eType_StencilEnable:
					//	{
					//		stateValue = jsnState.value.asBool();
					//		break;
					//	}

					//case RenderState::eType_SrcBlend:
					//	{
					//		stateValue = B_STRING_TO_ENUM(blendType, jsnState.value.asString());
					//		break;
					//	}

					//case RenderState::eType_DestBlend:
					//	{
					//		stateValue = B_STRING_TO_ENUM(blendType, jsnState.value.asString());
					//		break;
					//	}

					//case RenderState::eType_DepthBias:
					//	{
					//		stateValue = jsnState.value.asFloat();
					//		break;
					//	}

					//case RenderState::eType_AlphaRef:
					//	{
					//		stateValue = jsnState.value.asFloat();
					//		break;
					//	}

					//case RenderState::eType_FillMode:
					//	{
					//		stateValue = B_STRING_TO_ENUM(fillMode, jsnState.value.asString());
					//		break;
					//	}

					//case RenderState::eType_SlopeScaleDepthBias:
					//	{
					//		stateValue = jsnState.value.asFloat();
					//		break;
					//	}

					//case RenderState::eType_StencilFail:
					//	{
					//		stateValue = B_STRING_TO_ENUM(stencilOp, jsnState.value.asString());
					//		break;
					//	}

					//case RenderState::eType_StencilDepthFail:
					//	{
					//		stateValue = B_STRING_TO_ENUM(stencilOp, jsnState.value.asString());
					//		break;
					//	}

					//case RenderState::eType_StencilPass:
					//	{
					//		stateValue = B_STRING_TO_ENUM(stencilOp, jsnState.value.asString());
					//		break;
					//	}

					//case RenderState::eType_StencilFunc:
					//	{
					//		stateValue = B_STRING_TO_ENUM(cmpFunc, jsnState.value.asString());
					//		break;
					//	}

					//case RenderState::eType_StencilMask:
					//	{
					//		stateValue = jsnState.value.asFloat();
					//		break;
					//	}

					//case RenderState::eType_StencilWriteMask:
					//	{
					//		stateValue = jsnState.value.asFloat();
					//		break;
					//	}

					//case RenderState::eType_StencilRef:
					//	{
					//		stateValue = jsnState.value.asFloat();
					//		break;
					//	}

					//case RenderState::eType_DepthFunc:
					//	{
					//		stateValue = B_STRING_TO_ENUM(cmpFunc, jsnState.value.asString());
					//		break;
					//	}

					//case RenderState::eType_CullMode:
					//	{
					//		stateValue = B_STRING_TO_ENUM(cullMode, jsnState.value.asString());
					//		break;
					//	}
					//}

					file.writeFloat(stateValue);
				}
			}

			// constants count
			i16 constCount = jsnConstants ? jsnConstants->nameValuePairs().size() : 0;
			file.writeInt16(constCount);

			E_LOG_VERBOSE("Writing %d constants", constCount);

			if (jsnConstants)
			{
				for (size_t j = 0; j < jsnConstants->nameValuePairs().size(); ++j)
				{
					auto jsnConst = jsnConstants->nameValuePairs().at(j);

					file.writeString(jsnConst.key.c_str());
					file.writeString(jsnConst.value.asString());
				}
			}

			// texture sampler count
			i16 samplerCount = jsnSamplers ? jsnSamplers->arrayValues().size() : 0;
			file.writeInt16(samplerCount);

			E_LOG_VERBOSE("Writing %d texture samplers", samplerCount);

			if (jsnSamplers)
			{
				for (size_t j = 0; j < jsnSamplers->arrayValues().size(); ++j)
				{
					JsonNode::Value* jsnSampler = jsnSamplers->arrayValues().at(j);

					file.writeString(jsnSampler->asNode()->valueOf("sampler").asString());
					file.writeString(jsnSampler->asNode()->valueOf("file").asString());
					//file.writeInt32(B_STRING_TO_ENUM(textureAddress, jsnSampler->asNode()->valueOf("addressU").asString()));
					//file.writeInt32(B_STRING_TO_ENUM(textureAddress, jsnSampler->asNode()->valueOf("addressV").asString()));
					//file.writeInt32(B_STRING_TO_ENUM(textureAddress, jsnSampler->asNode()->valueOf("addressW").asString()));
					//file.writeInt32(B_STRING_TO_ENUM(textureFilter, jsnSampler->asNode()->valueOf("filter").asString()));
					file.writeFloat(jsnSampler->asNode()->valueOf("minLod").asFloat());
					file.writeFloat(jsnSampler->asNode()->valueOf("maxLod").asFloat());
					file.writeFloat(jsnSampler->asNode()->valueOf("mipMapLodBias").asFloat());
				}
			}
		}

		file.endChunk();// layers
	}

	E_LOG_VERBOSE("End write material layers");
	return true;
}

bool MaterialProcessor::process(const char* pSrcFilename, const char* pDestPath, const ArgsParser& rArgs)
{
	JsonDocument doc;

	if (!doc.loadAndParse(pSrcFilename))
	{
		return false;
	}
	
	String ext;

	extractFileExtension(pSrcFilename, ext);
	toLowerCase(ext);

	if (ext == ".material")
	{
		return convertMaterial(doc, pSrcFilename, pDestPath, rArgs);
	}

	return false;
}

bool MaterialProcessor::convertMaterial(
	JsonDocument& doc,
	const char* pSrcFilename,
	const char* pDestPath,
	const ArgsParser& rArgs)
{
	String filename, nameOnly;
	FileWriter file;

	extractFileNameNoExtension(pSrcFilename, nameOnly);
	filename = mergePathFile(pDestPath, (nameOnly + ".material").c_str());

	if (isFilesLastTimeSame(pSrcFilename, filename.c_str()))
	{
		B_LOG_DEBUG("Skipping %s", pSrcFilename);
		return true;
	}

	file.openFile(filename.c_str());
	//file.beginChunk(eMaterialChunk_Header, 1);
	file.writeString("MATERIAL");
	file.endChunk();

	bool bOk = writeMaterial(file, &doc.root());

	if (!bOk)
	{
		return false;
	}

	file.closeFile();
	setSameFileTime(pSrcFilename, filename.c_str());

	return true;
}

}