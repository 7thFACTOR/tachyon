#include "render/gpu_program_constant.h"
#include "core/globals.h"
#include "base/util.h"
#include "base/assert.h"
#include "base/logger.h"
#include "base/math/vec2.h"

namespace engine
{
//TODO: group constants in a continuous buffer in memory and not separate as new objects.

GpuProgramConstant::GpuProgramConstant()
{
	name = "";
	count = 1;
	data = nullptr;
	type = GpuProgramConstantType::Unknown;
}

GpuProgramConstant::GpuProgramConstant(const GpuProgramConstant& other)
{
	*this = other;
}

void GpuProgramConstant::set(
	GpuProgramConstantType type,
	const String& name,
	void* data,
	u32 count)
{
	this->name = name;
	this->count = count;
	this->type = type;
	setDataValue(data, count);
}

GpuProgramConstant::~GpuProgramConstant()
{
	freeData();
}

GpuProgramConstant& GpuProgramConstant::operator = (const GpuProgramConstant& other)
{
	freeData();
	this->name = other.name;
	this->count = other.count;
	this->type = other.type;
	setDataValue(other.getData(), count);

	return *this;
}

void GpuProgramConstant::setDataValue(void* data, u32 count)
{
	if (!this->data)
	{
		createData();
	}

	if (type == GpuProgramConstantType::Unknown)
	{
		B_ASSERT(!"Setting an unknown type gpu program constant");
		return;
	}

	switch (type)
	{
	case GpuProgramConstantType::Float:
		{
			if (count == 1)
			{
				*((f32*)this->data) = *((f32*)data);
			}
			else
			for (u32 i = 0; i < count; ++i)
			{
				((f32*)this->data)[i] = ((f32*)data)[i];
			}
			break;
		}

	case GpuProgramConstantType::Integer:
		{
			if (count == 1)
			{
				*((i32*)this->data) = *((i32*)data);
			}
			else
			for (u32 i = 0; i < count; ++i)
			{
				((i32*)this->data)[i] = ((i32*)data)[i];
			}
			break;
		}

	case GpuProgramConstantType::Matrix:
		{
			if (count == 1)
			{
				*((Matrix*)this->data) = *((Matrix*)data);
			}
			else
			for (u32 i = 0; i < count; ++i)
			{
				((Matrix*)this->data)[i] = ((Matrix*)data)[i];
			}
			break;
		}

	case GpuProgramConstantType::Vec3:
		{
			if (count == 1)
			{
				*((Vec3*)this->data) = *((Vec3*)data);
			}
			else
			for (u32 i = 0; i < count; ++i)
			{
				((Vec3*)this->data)[i] = ((Vec3*)data)[i];
			}
			break;
		}

	case GpuProgramConstantType::Vec2:
		{
			if (count == 1)
			{
				*((Vec2*)this->data) = *((Vec2*)data);
			}
			else
			for (u32 i = 0; i < count; ++i)
			{
				((Vec2*)this->data)[i] = ((Vec2*)data)[i];
			}
			break;
		}

	case GpuProgramConstantType::Color:
		{
			if (count == 1)
			{
				*((Color*)this->data) = *((Color*)data);
			}
			else
			for (u32 i = 0; i < count; ++i)
			{
				((Color*)this->data)[i] = ((Color*)data)[i];
			}
			break;
		}

	case GpuProgramConstantType::Bool:
		{
			if (count == 1)
			{
				*((bool*)this->data) = *((bool*)data);
			}
			else
			for (u32 i = 0; i < count; ++i)
			{
				((bool*)this->data)[i] = ((bool*)data)[i];
			}
			break;
		}

	case GpuProgramConstantType::Texture:
		{
			if (count == 1)
			{
				*((ResourceId*)this->data) = *((ResourceId*)data);
			}
			else
			for (u32 i = 0; i < count; ++i)
			{
				((ResourceId*)this->data)[i] = ((ResourceId*)data)[i];
			}
			break;
		}
	}
}

void GpuProgramConstant::createData()
{
	freeData();

	switch (type)
	{
	case GpuProgramConstantType::Float:
		{
			if (count > 1)
				data = new f32 [count];
			else
				data = new f32;

			B_ASSERT(data);
			dataSize = sizeof(f32) * count;
			break;
		}

	case GpuProgramConstantType::Integer:
		{
			if (count > 1)
				data = new i32 [count];
			else
				data = new i32;

			B_ASSERT(data);
			dataSize = sizeof(i32) * count;
			break;
		}

	case GpuProgramConstantType::Matrix:
		{
			if (count > 1)
				data = new Matrix [count];
			else
				data = new Matrix;

			B_ASSERT(data);
			dataSize = sizeof(Matrix) * count;
			break;
		}

	case GpuProgramConstantType::Vec2:
		{
			if (count > 1)
				data = new Vec2 [count];
			else
				data = new Vec2();

			B_ASSERT(data);
			dataSize = sizeof(Vec2) * count;
			break;
		}

	case GpuProgramConstantType::Vec3:
		{
			if (count > 1)
				data = new Vec3 [count];
			else
				data = new Vec3();

			B_ASSERT(data);
			dataSize = sizeof(Vec3) * count;
			break;
		}

	case GpuProgramConstantType::Color:
		{
			if (count > 1)
				data = new Color [count];
			else
				data = new Color();

			B_ASSERT(data);
			dataSize = sizeof(Color) * count;
			break;
		}

	case GpuProgramConstantType::Bool:
		{
			if (count > 1)
				data = new bool [count];
			else
				data = new bool;

			B_ASSERT(data);
			dataSize = sizeof(bool) * count;
			break;
		}

	case GpuProgramConstantType::Texture:
		{
			if (count > 1)
			{
				data = new ResourceId[count];

				for (size_t i = 0; i < count; ++i)
				{
					((ResourceId*)data)[i] = invalidResourceId;
				}
			}
			else
			{
				data = new ResourceId;
				ResourceId nullTex = invalidResourceId;
				setDataValue(&nullTex);
			}

			B_ASSERT(data);
			dataSize = sizeof(ResourceId) * count;
			break;
		}

	case GpuProgramConstantType::SystemTexture:
		{
			if (count > 1)
				data = new String[count];
			else
				data = new String();

			B_ASSERT(data);
			dataSize = 0;
			break;
		}
	}
}

void GpuProgramConstant::freeData()
{
	if (!data)
	{
		return;
	}

	switch (type)
	{
	case GpuProgramConstantType::Float:
		{
			if (count > 1)
				delete [](f32*) data;
			else
				delete(f32*) data;
			break;
		}

	case GpuProgramConstantType::Integer:
		{
			if (count > 1)
				delete [](i32*) data;
			else
				delete (i32*) data;
			break;
		}

	case GpuProgramConstantType::Matrix:
		{
			if (count > 1)
				delete [](Matrix*) data;
			else
				delete (Matrix*) data;
			break;
		}

	case GpuProgramConstantType::Vec2:
		{
			if (count > 1)
				delete [](Vec2*) data;
			else
				delete (Vec2*) data;
			break;
		}

	case GpuProgramConstantType::Vec3:
		{
			if (count > 1)
				delete [](Vec3*) data;
			else
				delete (Vec3*) data;
			break;
		}

	case GpuProgramConstantType::Color:
		{
			if (count > 1)
				delete [](Color*) data;
			else
				delete (Color*) data;
			break;
		}

	case GpuProgramConstantType::Bool:
		{
			if (count > 1)
				delete [](u32*) data;
			else
				delete (u32*) data;
			break;
		}

	case GpuProgramConstantType::Texture:
		{
			if (count > 1)
				delete [] (ResourceId*) data;
			else
				delete (ResourceId*)data;
			break;
		}

	case GpuProgramConstantType::SystemTexture:
		{
			delete (String*) data;
			break;
		}
	}

	data = nullptr;
}

}