#include <string.h>
#include <stdlib.h>
#include "render/gpu_program_render_method.h"
#include "resources/gpu_program_resource.h"
#include "render/graphics.h"
#include "core/globals.h"
#include "base/util.h"
#include "render/geometry.h"
#include "base/file.h"
#include "base/assert.h"
#include "base/logger.h"

namespace engine
{
GpuProgramRenderMethod::GpuProgramRenderMethod()
	: active(true)
	, asDefault(false)
{}

GpuProgramRenderMethod::~GpuProgramRenderMethod()
{
	free();
}

void GpuProgramRenderMethod::addConstant(
	const String& name,
	GpuProgramConstantType type,
	void* data,
	u32 count)
{
	GpuProgramConstant constant;

	constant.set(type, name, data, count);
	constants.append(constant);
}

void GpuProgramRenderMethod::removeConstant(const String& name)
{
	for (size_t i = 0, iCount = constants.size(); i < iCount; ++i)
	{
		if (constants[i].getName() == name)
		{
			constants.erase(constants.begin() + i);
			return;
		}
	}
}

void GpuProgramRenderMethod::setConstantValue(const String& constName, void* value, u32 count)
{
	GpuProgramConstant* constant = getConstantByName(constName);

	if (!constant)
	{
		B_LOG_WARNING("Could not find gpu program constant named: '%s' in render method '%s'",
			constName,
			name.c_str());
		return;
	}

	constant->setDataValue(value, count);
}

GpuProgramConstant* GpuProgramRenderMethod::getConstantByName(const String& name)
{
	for (size_t i = 0, iCount = constants.size(); i < iCount; ++i)
	{
		if (constants[i].getName() == name)
		{
			return &constants[i];
		}
	}

	return nullptr;
}

void GpuProgramRenderMethod::free()
{
	constants.clear();
}

ShaderOptions GpuProgramRenderMethod::getOptions() const
{
	return options;
}

void GpuProgramRenderMethod::setOptions(const ShaderOptions& options)
{
	this->options = options;
}

void GpuProgramRenderMethod::setActive(bool active)
{
	this->active = active;
}

bool GpuProgramRenderMethod::isActive() const
{
	return active;
}

void GpuProgramRenderMethod::setShaderSource(ShaderType shaderType, const String& source)
{
	shaderSource[(int)shaderType] = source;
}

const String& GpuProgramRenderMethod::getShaderSource(ShaderType shaderType) const
{
	return shaderSource[(int)shaderType];
}

const Array<GpuProgramConstant>& GpuProgramRenderMethod::getConstants() const
{
	return constants;
}

void GpuProgramRenderMethod::setDefault(bool value)
{
	asDefault = value;
}

bool GpuProgramRenderMethod::isDefault() const
{
	return asDefault;
}

const String& GpuProgramRenderMethod::getFallbackRenderMethodName() const
{
	return fallbackRenderMethodName;
}

}