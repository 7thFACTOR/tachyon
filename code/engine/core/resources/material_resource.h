#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/defines.h"
#include "base/types.h"
#include "core/resource.h"
#include "base/serialize.h"
#include "graphics/types.h"

namespace engine
{
const String wipMaterialFilename = "materials/wip.material";
const String missingMaterialFilename = "materials/missing.material";

struct GpuProgramConstantData;

struct MaterialResource : Resource
{
	ResourceId gpuProgramId = nullResourceId;
	RasterizerState rasterizerState;
	BlendState blendState;
	DepthStencilState depthStencilState;
	Array<GpuProgramConstantData> constants;
};

}