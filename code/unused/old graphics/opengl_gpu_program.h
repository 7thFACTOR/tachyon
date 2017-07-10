#pragma once
#include "opengl_globals.h"
#include "render/resources/gpu_program_resource.h"
#include "render/mesh.h"
#include "render/gpu_program.h"

namespace engine
{
class OpenGLGraphics;

struct OpenGLVertexAttribute
{
	String name;
	GpuBufferElementType semantic;
	i32 location;
	i32 index;
};

typedef Array<OpenGLVertexAttribute> OpenGLVertexAttributes;

class OpenGLGpuProgram : public GpuProgram
{
public:
	friend class OpenGLGraphics;
	
	OpenGLGpuProgram();
	~OpenGLGpuProgram();

	void bind() override;
	void unbind() override;
	void setResource(ResourceId resId) override;
	ResourceId getResourceId() const override { return resource; }
	void setDefaultConstants();
	void setConstant(const GpuProgramConstantData& constant, u32& textureUnit);
	void setMatrixUniform(const String& constName, const Matrix& mtx) override;
	void setMatrixArrayUniform(const String& constName, const Matrix* matrices, u32 count) override;
	void setIntUniform(const String& constName, i32 value) override;
	void setIntArrayUniform(const String& constName, i32* intValues, u32 count) override;
	void setFloatUniform(const String& constName, f32 value) override;
	void setFloatArrayUniform(const String& constName, f32* floatValues, u32 count) override;
	void setSamplerUniform(Texture* tex, const String& constName, i32 stage) override;

	OpenGLVertexAttribute* findAttribute(GpuBufferElementType type, u32 index = 0)
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

	inline GLuint getProgramId() const { return oglProgram; }

protected:
	void free();
	void fillAttributes();

	ResourceId resource = nullResourceId;
	GLuint oglPixelShader = 0;
	GLuint oglVertexShader = 0;
	GLuint oglGeometryShader = 0;
	GLuint oglTesselationControlShader = 0;
	GLuint oglTesselationEvaluationShader = 0;
	GLuint oglProgram = 0;
	OpenGLVertexAttributes oglAttributes = 0;
};

}