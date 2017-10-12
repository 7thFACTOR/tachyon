// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "../common.h"
#include "../asset_processor.h"
#include "base/math/vec3.h"
#include "base/math/bbox.h"

#define AC_USE_OPENGL
//#define AC_USE_VULKAN

#ifdef AC_USE_OPENGL
#include "Cg/cg.h"
#include "Cg/cgGL.h"
#endif

#include "base/json.h"
#include "core/resources/gpu_program_resource.h"
#include "graphics/types.h"

namespace ac
{
using namespace engine;

B_DECLARE_ENUM_STRINGIFY(GpuProgramConstantType);

class GpuProgramProcessor : public AssetProcessor
{
public:
	GpuProgramProcessor();
	~GpuProgramProcessor();

	bool import(const String& importFilename, JsonDocument& assetCfg) override;
	bool process(Asset& asset, JsonDocument& assetCfg) override;
	bool isModified(Asset& asset) override;

protected:
	void compileAndWriteProgram(
		CGcontext context,
		Stream* stream,
		const char** args,
		CGprofile profile,
		const String& programSrc,
		const String& mainEntry);

		//TODO: just use GLSL or HLSL, Cg is not supported by NVIDIA anymore

#ifdef AC_USE_OPENGL
	CGcontext cgContextGL;
#endif
};
}