#pragma once
#include "graphics/gpu_buffer.h"

namespace engine
{
struct OpenGLVertexAttribute
{
	String name;
	GpuBufferElementType semantic;
	i32 location;
	i32 index;
};

typedef Array<OpenGLVertexAttribute> OpenGLVertexAttributes;

class E_API OpenglGpuBuffer : public GpuBuffer
{
public:
	OpenglGpuBuffer();
	~OpenglGpuBuffer();
	void initialize(u32 dataSize = 0, u8* data = nullptr) override;
	u8* map(u32 offset, u32 size, GpuBufferMapType mapType) override;
	void unmap() override;
	void bind() override;
	void unbind() override;
	void resize(u32 newSize, u8* newData = nullptr) override;

	void setupVertexDeclarationGL(class OpenglGpuProgram& program, u32 offset = 0);
	void freeGL();

#ifdef _DEBUG
	void debug();
#endif

public:
	GLuint oglBufferId = 0;
	u32 oglLastUsedAttribIndex = 0;
	Array<u8> oglMappedRangeBuffer;
};

}