#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/array.h"
#include "base/string.h"
#include "base/serialize.h"
#include "base/class_registry.h"
#include "base/stream.h"
#include "render/gpu_program_constant.h"
#include "render/gpu_buffer_descriptor.h"

namespace engine
{
using namespace base;

class GpuProgram;

class ENGINE_API GpuProgramRenderMethod
{
public:
	GpuProgramRenderMethod();
	virtual ~GpuProgramRenderMethod();
	void free();
	void setActive(bool active);
	bool isActive() const;
	void setShaderSource(ShaderType shader, const String& source);
	const String& getShaderSource(ShaderType shader) const;
	const Array<GpuProgramConstant>& getConstants() const;
	const GpuBufferDescriptor& getGpuBufferDesc() const;
	void setGpuBufferDesc(const GpuBufferDescriptor& desc);
	void addConstant(const String& name, GpuProgramConstantType type, void* data = nullptr, u32 count = 0);
	void removeConstant(const String& name);
	GpuProgramConstant* getConstantByName(const String& name);
	void setConstantValue(const String& constName, void* value, u32 count = 1);

	template<typename Type>
	void setConstantValue(const String& constName, Type& value, u32 count = 1)
	{
		setConstantValue(constName, &value, count);
	}

	ShaderOptions getOptions() const;
	void setOptions(const ShaderOptions& options);
	void setDefault(bool value);
	bool isDefault() const;
	const String& getFallbackRenderMethodName() const;
	void setFallbackRenderMethodName(const String& name) { fallbackRenderMethodName = name; }
	const String& getName() const { return name; }
	void setName(const String& name) { this->name = name; }

protected:
	String name;
	bool active;
	bool asDefault;
	ShaderOptions options;
	String shaderSource[(u32)ShaderType::Count];
	String fallbackRenderMethodName;
	Array<GpuProgramConstant> constants;
};

}