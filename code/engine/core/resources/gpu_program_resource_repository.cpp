#include "core/resources/gpu_program_resource_repository.h"
#include "base/defines.h"
#include "base/assert.h"
#include "base/logger.h"
#include "core/globals.h"
#include "graphics/graphics.h"
#include "base/util.h"
#include "core/resource_repository.h"

namespace engine
{
bool GpuProgramResourceRepository::load(Stream& stream, ResourceId resId)
{
	GpuProgramResource& program = *resources[resId];
	GraphicsApiType api;

	stream >> api;

	if (api != getGraphics().getApiType())
	{
		B_LOG_ERROR("Gpu program was compiled for another graphics api. ResourceId: " << resId);
		return false;
	}

	stream >> program.options;

	for (u32 i = 0; i < (u32)ShaderType::Count; ++i)
	{
		stream >> program.shaderSource[i];
	}

	u32 constantCount = 0;
	stream >> constantCount;

	program.constants.resize(constantCount);

	for (u32 m = 0; m < constantCount; m++)
	{
		auto& constant = program.constants[m];
						
		stream >> constant.valueCount;
		stream >> constant.type;
		stream >> constant.name;

		switch (constant.type)
		{
		case GpuProgramConstantType::Color:
			constant.data = (u8*)new Color[constant.valueCount];
			constant.dataSize = sizeof(Color) * constant.valueCount;
			break;
		case GpuProgramConstantType::Float:
			constant.data = (u8*)new f32[constant.valueCount];
			constant.dataSize = sizeof(f32) * constant.valueCount;
			break;
		case GpuProgramConstantType::Integer:
			constant.data = (u8*)new i32[constant.valueCount];
			constant.dataSize = sizeof(i32) * constant.valueCount;
			break;
		case GpuProgramConstantType::Matrix:
			constant.data = (u8*)new Matrix[constant.valueCount];
			constant.dataSize = sizeof(Matrix) * constant.valueCount;
			break;
		case GpuProgramConstantType::Texture:
			constant.data = (u8*)new ResourceId[constant.valueCount];
			constant.dataSize = sizeof(ResourceId) * constant.valueCount;
			break;
		case GpuProgramConstantType::Vec2:
			constant.data = (u8*)new Vec2[constant.valueCount];
			constant.dataSize = sizeof(Vec2) * constant.valueCount;
			break;
		case GpuProgramConstantType::Vec3:
			constant.data = (u8*)new Vec3[constant.valueCount];
			constant.dataSize = sizeof(Vec3) * constant.valueCount;
			break;
		default:
			constant.data = nullptr;
			constant.dataSize = 0;
			break;
		}

		B_LOG_DEBUG("\tConstant: " << constant.name << " type: " << (u32)constant.type << " valueCount: " << (u32)constant.valueCount << " dataSize: " << (u64)constant.dataSize);

		if (constant.data && constant.dataSize)
		{
			// read the default value for the constant
			stream.read(constant.data, constant.dataSize);

			if (constant.type == GpuProgramConstantType::Texture)
			{
				ResourceId texId = *(u64*)constant.data;

				// start to load that texture
				if (texId != nullResourceId)
				{
					getResources().load(texId);
				}
			}
		}
	}

	return true;
}

void GpuProgramResourceRepository::unload(ResourceId resId)
{
	GpuProgramResource* program = resources[resId];

	if (!program)
	{
		return;
	}

	for (size_t j = 0; j < program->constants.size(); j++)
	{
		delete [] program->constants[j].data;
	}

	program->constants.clear();
	resources.erase(resId);
}

}