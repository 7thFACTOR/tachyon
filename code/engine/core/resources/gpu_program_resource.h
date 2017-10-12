// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/array.h"
#include "base/string.h"
#include "base/defines.h"
#include "base/types.h"
#include "core/resource.h"
#include "base/variant.h"
#include "graphics/types.h"

namespace engine
{
struct GpuProgramConstantData
{
	//! the constant type
	GpuProgramConstantType type = GpuProgramConstantType::Unknown;
	//! the constant name
	String name;
	//! the number of values in the data
	u32 valueCount = 0;
	u32 dataSize = 0;
	u8* data = nullptr;
};

struct GpuProgramResource : Resource
{
	GpuProgramOptions options = 0;
	String shaderSource[(u32)ShaderType::Count];
	Array<GpuProgramConstantData> constants;
};

}