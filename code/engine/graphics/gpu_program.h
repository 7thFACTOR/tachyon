// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/math/matrix.h"
#include "base/math/vec2.h"
#include "base/math/vec3.h"
#include "base/math/color.h"
#include "graphics/types.h"
#include "graphics/gpu_buffer.h"
#include "base/array.h"

namespace base
{
class String;
}

namespace engine
{
using namespace base;
class Texture;
struct GpuProgramConstantData;

class E_API GpuProgram
{
public:
	GpuProgram() {}
	virtual ~GpuProgram() {}
	virtual void bind() = 0;
	virtual void unbind() = 0;
	virtual void setFromResource(ResourceId resId) = 0;
	inline ResourceId getResourceId() const { return resourceId; }
	void setDefaultConstants();
	void setConstant(const GpuProgramConstantData& constant, u32& textureUnit);
	virtual void setVec2Uniform(const String& constName, const Vec2& val) = 0;
	virtual void setVec3Uniform(const String& constName, const Vec3& val) = 0;
	virtual void setColorUniform(const String& constName, const Color& val) = 0;
	virtual void setMatrixUniform(const String& constName, const Matrix& mtx) = 0;
	virtual void setMatrixArrayUniform(const String& constName, const Matrix* matrices, u32 count) = 0;
	virtual void setIntUniform(const String& constName, i32 value) = 0;
	virtual void setIntArrayUniform(const String& constName, i32* intValues, u32 count) = 0;
	virtual void setFloatUniform(const String& constName, f32 value) = 0;
	virtual void setFloatArrayUniform(const String& constName, f32* floatValues, u32 count) = 0;
	virtual void setSamplerUniform(Texture* tex, const String& constName, i32 stage) = 0;

protected:
	ResourceId resourceId = nullResourceId;
};

}