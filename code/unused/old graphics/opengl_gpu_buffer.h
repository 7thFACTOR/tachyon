#pragma once
#include "opengl_globals.h"
#include "render/gpu_buffer.h"

namespace engine
{
class OpenGLGpuProgram;

class OpenGLGpuBuffer : public GpuBuffer
{
public:
	friend class OpenGLGraphics;

	OpenGLGpuBuffer();
	~OpenGLGpuBuffer();

	void initialize(u32 size = 0, void* data = nullptr) override;
	void* map(u32 offset, u32 size, GpuBufferMapType mapType = GpuBufferMapType::Write) override;
	void unmap() override;
	void bind() override;
	void unbind() override;
	void resize(u32 size, void* data = nullptr) override;
	void free();
	void setupVertexDeclarationGL(OpenGLGpuProgram& program, u32 offset = 0);
#ifndef NDEBUG
	void debug();
#endif

protected:
	GLuint oglBufferId = 0;
	u32 oglLastUsedAttribIndex = 0;
	Array<u8> oglMappedRangeBuffer;
	u32 bufferSize = 0;
};

}