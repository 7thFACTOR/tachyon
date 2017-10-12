// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "material_processor.h"
#include "base/logger.h"
#include "base/platform.h"
#include "base/util.h"
#include "base/json.h"
#include "core/resources/texture_resource.h"
#include "core/resources/material_resource.h"
#include "core/types.h"
#include "../project.h"
#include "base/file.h"
#include "gpu_program_processor.h"
#include "core/resources/gpu_program_resource.h"
#include "graphics/types.h"
#include "base/math/util.h"

namespace ac
{
using namespace base;
using namespace engine;

B_BEGIN_ENUM_STRINGIFY(CompareFunction)
	B_ADD_ENUM_STRINGIFY("never", CompareFunction::Never)
	B_ADD_ENUM_STRINGIFY("less", CompareFunction::Less)
	B_ADD_ENUM_STRINGIFY("equal", CompareFunction::Equal)
	B_ADD_ENUM_STRINGIFY("lessEqual", CompareFunction::LessEqual)
	B_ADD_ENUM_STRINGIFY("greater", CompareFunction::Greater)
	B_ADD_ENUM_STRINGIFY("notEqual", CompareFunction::NotEqual)
	B_ADD_ENUM_STRINGIFY("greaterEqual", CompareFunction::GreaterEqual)
	B_ADD_ENUM_STRINGIFY("always", CompareFunction::Always)
B_END_ENUM_STRINGIFY(CompareFunction)

B_BEGIN_ENUM_STRINGIFY(BlendType)
	B_ADD_ENUM_STRINGIFY("zero", BlendType::Zero)
	B_ADD_ENUM_STRINGIFY("one", BlendType::One)
	B_ADD_ENUM_STRINGIFY("srcColor", BlendType::SrcColor)
	B_ADD_ENUM_STRINGIFY("invSrcColor", BlendType::InvSrcColor)
	B_ADD_ENUM_STRINGIFY("srcAlpha", BlendType::SrcAlpha)
	B_ADD_ENUM_STRINGIFY("invSrcAlpha", BlendType::InvSrcAlpha)
	B_ADD_ENUM_STRINGIFY("dstAlpha", BlendType::DstAlpha)
	B_ADD_ENUM_STRINGIFY("invDstAlpha", BlendType::InvDstAlpha)
	B_ADD_ENUM_STRINGIFY("dstColor", BlendType::DstColor)
	B_ADD_ENUM_STRINGIFY("invDstColor", BlendType::InvDstColor)
	B_ADD_ENUM_STRINGIFY("srcAlphaSat", BlendType::SrcAlphaSat)
B_END_ENUM_STRINGIFY(BlendType)

B_BEGIN_ENUM_STRINGIFY(RenderState)
	B_ADD_ENUM_STRINGIFY("", RenderState::None)
	B_ADD_ENUM_STRINGIFY("alphaBlendEnable", RenderState::AlphaBlendEnable)
	B_ADD_ENUM_STRINGIFY("alphaRef", RenderState::AlphaRef)
	B_ADD_ENUM_STRINGIFY("alphaTestEnable", RenderState::AlphaTestEnable)
	B_ADD_ENUM_STRINGIFY("colorWriteEnable", RenderState::ColorWriteEnable)
	B_ADD_ENUM_STRINGIFY("depthBias", RenderState::DepthBias)
	B_ADD_ENUM_STRINGIFY("destBlend", RenderState::DestBlend)
	B_ADD_ENUM_STRINGIFY("fillMode", RenderState::FillMode)
	B_ADD_ENUM_STRINGIFY("slopeScaleDepthBias", RenderState::SlopeScaleDepthBias)
	B_ADD_ENUM_STRINGIFY("srcBlend", RenderState::SrcBlend)
	B_ADD_ENUM_STRINGIFY("stencilEnable", RenderState::StencilEnable)
	B_ADD_ENUM_STRINGIFY("stencilFail", RenderState::StencilFail)
	B_ADD_ENUM_STRINGIFY("stencilFunc", RenderState::StencilFunc)
	B_ADD_ENUM_STRINGIFY("stencilMask", RenderState::StencilMask)
	B_ADD_ENUM_STRINGIFY("stencilPass", RenderState::StencilPass)
	B_ADD_ENUM_STRINGIFY("stencilRef", RenderState::StencilRef)
	B_ADD_ENUM_STRINGIFY("stencilWriteMask", RenderState::StencilWriteMask)
	B_ADD_ENUM_STRINGIFY("stencilDepthFail", RenderState::StencilDepthFail)
	B_ADD_ENUM_STRINGIFY("depthTestEnable", RenderState::DepthTestEnable)
	B_ADD_ENUM_STRINGIFY("depthWriteEnable", RenderState::DepthWriteEnable)
	B_ADD_ENUM_STRINGIFY("depthFunc", RenderState::DepthFunc)
	B_ADD_ENUM_STRINGIFY("cullMode", RenderState::CullMode)
B_END_ENUM_STRINGIFY(RenderState)

B_BEGIN_ENUM_STRINGIFY(FillMode)
	B_ADD_ENUM_STRINGIFY("point", FillMode::Point)
	B_ADD_ENUM_STRINGIFY("wireframe", FillMode::Wireframe)
	B_ADD_ENUM_STRINGIFY("solid", FillMode::Solid)
B_END_ENUM_STRINGIFY(FillMode)

B_BEGIN_ENUM_STRINGIFY(StencilOperation)
	B_ADD_ENUM_STRINGIFY("keep", StencilOperation::Keep)
	B_ADD_ENUM_STRINGIFY("zero", StencilOperation::Zero)
	B_ADD_ENUM_STRINGIFY("replace", StencilOperation::Replace)
	B_ADD_ENUM_STRINGIFY("incrSat", StencilOperation::IncrSat)
	B_ADD_ENUM_STRINGIFY("decrSat", StencilOperation::DecrSat)
	B_ADD_ENUM_STRINGIFY("invert", StencilOperation::Invert)
	B_ADD_ENUM_STRINGIFY("incr", StencilOperation::Incr)
	B_ADD_ENUM_STRINGIFY("decr", StencilOperation::Decr)
B_END_ENUM_STRINGIFY(StencilOperation)

B_BEGIN_ENUM_STRINGIFY(CullMode)
	B_ADD_ENUM_STRINGIFY("none", CullMode::None)
	B_ADD_ENUM_STRINGIFY("cw", CullMode::CW)
	B_ADD_ENUM_STRINGIFY("ccw", CullMode::CCW)
B_END_ENUM_STRINGIFY(CullMode)

MaterialProcessor::MaterialProcessor()
{
	Array<String> extensions;

	extensions.append(".material");
	supportedAssetType = SupportedAssetType(ResourceType::Material, extensions);
}

MaterialProcessor::~MaterialProcessor()
{}

bool MaterialProcessor::import(const String& importFilename, JsonDocument& assetCfg)
{
	assetCfg.addValue("type", "material");

	return true;
}

bool MaterialProcessor::process(Asset& asset, JsonDocument& assetCfg)
{
	auto cfg = asset.project->currentBuildConfig->processorConfigs["material"].cfg;
	File file;
	JsonDocument mtl(asset.absFilename);
	ResourceId gpuProgramId = toResourceId(mtl.getString("gpuProgram"));

	file.open(asset.deployFilename, FileOpenFlags::BinaryWrite);
	file << gpuProgramId;
	auto& states = mtl.getObject("states")->getMembers();

	asset.dependencies.appendUnique(gpuProgramId);
	
	file << (u32)states.size();

	for (auto& state : states)
	{
		f32 fStateValue;
		i32 iStateValue;
		bool isFloat = false;
		RenderState stateType = (RenderState)B_STRING_TO_ENUM(RenderState, state.key);
		
		file << stateType;
		
		switch (stateType)
		{
		case RenderState::AlphaBlendEnable:
		case RenderState::AlphaTestEnable:
		case RenderState::ColorWriteEnable:
		case RenderState::DepthTestEnable:
		case RenderState::DepthWriteEnable:
		case RenderState::StencilEnable:
			{
				iStateValue = state.value->asBool();
				break;
			}

		case RenderState::SrcBlend:
			{
				iStateValue = B_STRING_TO_ENUM(BlendType, state.value->asString());
				break;
			}

		case RenderState::DestBlend:
			{
				iStateValue = B_STRING_TO_ENUM(BlendType, state.value->asString());
				break;
			}

		case RenderState::DepthBias:
			{
				isFloat = true;
				fStateValue = state.value->asF32();
				break;
			}

		case RenderState::AlphaRef:
			{
				isFloat = true;
				fStateValue = state.value->asF32();
				break;
			}

		case RenderState::FillMode:
			{
				iStateValue = B_STRING_TO_ENUM(FillMode, state.value->asString());
				break;
			}

		case RenderState::SlopeScaleDepthBias:
			{
				isFloat = true;
				fStateValue = state.value->asF32();
				break;
			}

		case RenderState::StencilFail:
			{
				iStateValue = B_STRING_TO_ENUM(StencilOperation, state.value->asString());
				break;
			}

		case RenderState::StencilDepthFail:
			{
				iStateValue = B_STRING_TO_ENUM(StencilOperation, state.value->asString());
				break;
			}

		case RenderState::StencilPass:
			{
				iStateValue = B_STRING_TO_ENUM(StencilOperation, state.value->asString());
				break;
			}

		case RenderState::StencilFunc:
			{
				iStateValue = B_STRING_TO_ENUM(CompareFunction, state.value->asString());
				break;
			}

		case RenderState::StencilMask:
			{
				iStateValue = state.value->asI32();
				break;
			}

		case RenderState::StencilWriteMask:
			{
				iStateValue = state.value->asI32();
				break;
			}

		case RenderState::StencilRef:
			{
				isFloat = true;
				fStateValue = state.value->asF32();
				break;
			}

		case RenderState::DepthFunc:
			{
				iStateValue = B_STRING_TO_ENUM(CompareFunction, state.value->asString());
				break;
			}

		case RenderState::CullMode:
			{
				iStateValue = B_STRING_TO_ENUM(CullMode, state.value->asString());
				break;
			}
	
		default:
			{
				iStateValue = 0;
				break;
			}
		}

		file << isFloat;

		if (isFloat)
		{
			file << fStateValue;
		}
		else
		{
			file << iStateValue;
		}
	}

	auto& constants = mtl.getObject("constants")->getMembers();

	file << (u32)constants.size();

	// we need to load the assigned gpu program so we can extract constants info
	JsonDocument docGpuProgram;
	String programFilename = mergePathName(asset.bundle->absPath, mtl.getString("gpuProgram"));

	if (!docGpuProgram.loadAndParse(programFilename))
	{
		B_LOG_ERROR("Cannot load and parse the gpu program for constants: '" << programFilename << "'");
		return false;
	}

	struct GpuProgramConstInfo
	{
		GpuProgramConstantType type = GpuProgramConstantType::Unknown;
		String name;
		u32 count = 0;
	};

	auto gpuConstants = docGpuProgram.getObject("constants");
	
	Dictionary<String, GpuProgramConstInfo> gpuConstantsInfo;

	if (gpuConstants)
	{
		for (size_t i = 0; i < gpuConstants->getMembers().size(); i++)
		{
			auto gpuConst = gpuConstants->getMembers()[i].value->asObject();
			GpuProgramConstInfo info;

			String constTypeName = gpuConst->getString("type");
			info.type = (GpuProgramConstantType)B_STRING_TO_ENUM(GpuProgramConstantType, constTypeName);
			info.name = gpuConstants->getMembers()[i].key;
			info.count = gpuConst->getU32("count");
			gpuConstantsInfo.add(info.name, info);
		}
	}

	for (auto& constant : constants)
	{
		auto iter = gpuConstantsInfo.find(constant.key);

		if (iter == gpuConstantsInfo.end())
		{
			B_LOG_ERROR("Material constant '" << constant.key << "' not found in gpu program '" << mtl.getString("gpuProgram") << "'");
			return false;
		}

		file << iter->value.type;
		file << constant.key;
		
		u32 dataSize = 0;
		u32 ccount = iter->value.count;
		GpuProgramConstantType ctype = iter->value.type;
		Array<String> values;

		switch (ctype)
		{
		case GpuProgramConstantType::Unknown:
			B_LOG_ERROR("Material constant type is Unknown");
			return false;
			break;

		case GpuProgramConstantType::Float:
		{
			explodeString(constant.value->toString(), values, "|");
			
			if (values.size() > 1)
			{
				dataSize = sizeof(f32) * values.size();
				file << (u32)values.size();
				file << dataSize;

				for (size_t n = 0; n < values.size(); n++)
				{
					file << values[n].asF32();
				}
			}
			else
			{
				file << (u32)1;
				file << (u32)sizeof(f32);
				file << (f32)constant.value->asF32();
			}

			break;
		}
		case GpuProgramConstantType::Integer:
			explodeString(constant.value->toString(), values, "|");
			
			if (values.size() > 1)
			{
				dataSize = sizeof(i32) * values.size();
				file << (u32)values.size();
				file << dataSize;

				for (size_t n = 0; n < values.size(); n++)
				{
					file << values[n].asInt();
				}
			}
			else
			{
				file << (u32)1;
				file << (u32)sizeof(i32);
				file << constant.value->asI32();
			}

			break;

		case GpuProgramConstantType::Matrix:
			explodeString(constant.value->asString(), values, "|");
			dataSize = sizeof(Matrix) * values.size();
			file << (u32)values.size();
			file << dataSize;

			for (size_t n = 0; n < values.size(); n++)
			{
				file << toMatrix(values[n]);
			}
			break;

		case GpuProgramConstantType::Vec2:
			explodeString(constant.value->asString(), values, "|");
			dataSize = sizeof(Vec2) * values.size();
			file << (u32)values.size();
			file << dataSize;

			for (size_t n = 0; n < values.size(); n++)
			{
				file << toVec2(values[n]);
			}
			break;

		case GpuProgramConstantType::Vec3:
			explodeString(constant.value->asString(), values, "|");
			dataSize = sizeof(Vec3) * values.size();
			file << (u32)values.size();
			file << dataSize;

			for (size_t n = 0; n < values.size(); n++)
			{
				file << toVec3(values[n]);
			}
			break;

		case GpuProgramConstantType::Color:
			explodeString(constant.value->asString(), values, "|");
			dataSize = sizeof(Color) * values.size();
			file << (u32)values.size();
			file << dataSize;

			for (size_t n = 0; n < values.size(); n++)
			{
				file << toColor(values[n]);
			}
			break;

		case GpuProgramConstantType::Texture:
			explodeString(constant.value->asString(), values, "|");
			dataSize = sizeof(ResourceId) * values.size();
			file << (u32)values.size();
			file << dataSize;

			for (size_t n = 0; n < values.size(); n++)
			{
				file << (u64)toResourceId(values[n]);
				asset.dependencies.appendUnique(toResourceId(values[n]));
			}
			break;

		default:
			break;
		}
	}

	file.close();

	return true;
}

}