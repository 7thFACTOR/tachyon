#include "render/index_buffer.h"

namespace engine
{
IndexBuffer::IndexBuffer()
{
	m_primitiveType = RenderPrimitive::Triangles;
}

IndexBuffer::~IndexBuffer()
{}

}