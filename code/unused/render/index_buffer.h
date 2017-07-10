#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/class_registry.h"
#include "render/types.h"
#include "render/gpu_buffer.h"

namespace engine
{
//! Instance buffer is used to render more instances of a geometry buffer\n
//! See in examples folder some usage of this class
class E_API IndexBuffer : public GpuBuffer
{
protected:
	IndexBuffer();

public:
	virtual ~IndexBuffer();

	RenderPrimitive primitiveType() const { return m_primitiveType; }
	void setPrimitiveType(RenderPrimitive type) { m_primitiveType = type; }

protected:
	RenderPrimitive m_primitiveType;
};

}