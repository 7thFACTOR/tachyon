#pragma once
#include "common.h"
#include "render/index_buffer.h"

namespace engine
{
class GLIndexBuffer : public IndexBuffer
{
public:
	friend class GLRenderer;

	GLIndexBuffer();
	~GLIndexBuffer();

	void initialize(u32 size = 0, void* pData = nullptr) override;
	void* map(u32 offset, u32 size, GpuBufferMapType mapType = GpuBufferMapType::Write) override;
	void unmap() override;
	void bind() override;
	void unbind() override;
	void resize(u32 size, void* pData = nullptr) override;
	void free();

protected:
	GLuint m_glBufferId;
};

}