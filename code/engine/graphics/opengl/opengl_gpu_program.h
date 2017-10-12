// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "graphics/gpu_program.h"
#include "graphics/opengl/opengl_gpu_buffer.h"

#ifdef _WINDOWS
#include <windows.h>
#endif

#include <GL/gl.h>

namespace engine
{
class E_API OpenglGpuProgram : public GpuProgram
{
public:
	OpenglGpuProgram();
	~OpenglGpuProgram();
	void bind() override;
	void unbind() override;
	void setFromResource(ResourceId resId) override;
	void setVec2Uniform(const String& constName, const Vec2& val) override;;
	void setVec3Uniform(const String& constName, const Vec3& val) override;
	void setColorUniform(const String& constName, const Color& val) override;;
	void setMatrixUniform(const String& constName, const Matrix& mtx) override;;
	void setMatrixArrayUniform(const String& constName, const Matrix* matrices, u32 count) override;;
	void setIntUniform(const String& constName, i32 value) override;
	void setIntArrayUniform(const String& constName, i32* intValues, u32 count) override;
	void setFloatUniform(const String& constName, f32 value) override;
	void setFloatArrayUniform(const String& constName, f32* floatValues, u32 count) override;
	void setSamplerUniform(Texture* tex, const String& constName, i32 stage) override;

	OpenGLVertexAttribute* findAttributeGL(GpuBufferElementType type, u32 index = 0)
	{
		for (size_t i = 0; i < oglAttributes.size(); ++i)
		{
			if (oglAttributes[i].semantic == type && oglAttributes[i].index == index)
			{
				return &oglAttributes[i];
			}
		}

		return nullptr;
	}

protected:
	void freeGL();
	void fillAttributesGL();

	GLuint oglPixelShader = 0;
	GLuint oglVertexShader = 0;
	GLuint oglGeometryShader = 0;
	GLuint oglTesselationControlShader = 0;
	GLuint oglTesselationEvaluationShader = 0;
	GLuint oglProgram = 0;
	OpenGLVertexAttributes oglAttributes = 0;
};

}
