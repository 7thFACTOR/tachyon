#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "render/types.h"
#include "render/gpu_buffer.h"

namespace engine
{
class E_API VertexBuffer : public GpuBuffer
{
protected:
	VertexBuffer();

public:
	virtual ~VertexBuffer();

	void setAsInstanceBuffer(bool value) { m_bInstancing = value; }
	bool isInstanceBuffer() const { return m_bInstancing; }

protected:
	bool m_bInstancing;
};

}