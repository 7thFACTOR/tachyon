// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "core/globals.h"
#include "core/resource.h"
#include "core/resources/gpu_program_resource.h"
#include "core/resource_repository.h"
#include "graphics/graphics.h"
#include "graphics/gpu_program.h"

namespace engine
{
void GpuProgram::setDefaultConstants()
{
	ScopedResourceMapping<GpuProgramResource> programRes(resourceId);

	if (!programRes)
	{
		return;
	}

	u32 texUnit = 0;

	for (size_t i = 0; i < programRes->constants.size(); i++)
	{
		GpuProgramConstantData& constant = programRes->constants[i];
		setConstant(constant, texUnit);
	}
}

void GpuProgram::setConstant(const GpuProgramConstantData& constant, u32& textureUnit)
{
	switch (constant.type)
	{
	case GpuProgramConstantType::Unknown:
		break;
	case GpuProgramConstantType::Float:
	{
		f32* values = (f32*)constant.data;

		if (constant.valueCount == 1)
		{
			setFloatUniform(constant.name, values[0]);
		}
		else
		{
			setFloatArrayUniform(constant.name, values, constant.valueCount);
		}
		break;
	}

	case GpuProgramConstantType::Integer:
	{
		i32* values = (i32*)constant.data;

		if (constant.valueCount == 1)
		{
			setIntUniform(constant.name, values[0]);
		}
		else
		{
			setIntArrayUniform(constant.name, values, constant.valueCount);
		}
		break;
	}

	case GpuProgramConstantType::Matrix:
	{
		Matrix* values = (Matrix*)constant.data;

		if (constant.valueCount == 1)
		{
			setMatrixUniform(constant.name, values[0]);
		}
		else
		{
			setMatrixArrayUniform(constant.name, values, constant.valueCount);
		}
		break;
	}

	case GpuProgramConstantType::Vec2:
	{
		f32* values = (f32*)constant.data;

		if (constant.valueCount == 1)
		{
			setFloatArrayUniform(constant.name, values, 2);
		}
		else
		{
			setFloatArrayUniform(constant.name, values, constant.valueCount * 2);
		}
		break;
	}

	case GpuProgramConstantType::Vec3:
	{
		f32* values = (f32*)constant.data;

		if (constant.valueCount == 1)
		{
			setFloatArrayUniform(constant.name, values, 3);
		}
		else
		{
			setFloatArrayUniform(constant.name, values, constant.valueCount * 3);
		}
		break;
	}

	case GpuProgramConstantType::Color:
	{
		f32* values = (f32*)constant.data;

		if (constant.valueCount == 1)
		{
			setFloatArrayUniform(constant.name, values, 4);
		}
		else
		{
			setFloatArrayUniform(constant.name, values, constant.valueCount * 4);
		}
		break;
	}

	case GpuProgramConstantType::Texture:
	{
		u64* values = (u64*)constant.data;

		if (constant.valueCount == 1)
		{
			Texture* tex = getGraphics().textures[values[0]];
			
			if (!tex)
				tex = getGraphics().textures[loadResource("textures/missing_texture.png")];

			if (tex)
			{
				setSamplerUniform(tex, constant.name, textureUnit++);
			}
		}
		else
		{
			for (size_t t = 0; t < constant.valueCount; t++)
			{
				Texture* tex = getGraphics().textures[values[t]];

				if (tex)
				{
					setSamplerUniform(tex, constant.name, textureUnit++);
				}
			}
		}
		break;
	}

	default:
		getGraphics().setBuiltInGpuProgramConstant(this, constant.type);
	}
}

}