#include "common.h"
#ifdef B_PLATFORM_WIN
#include <windows.h>
#endif
#include "gl_index_buffer.h"
#include "base/logger.h"
#include "base/util.h"

namespace engine
{
GLIndexBuffer::GLIndexBuffer()
	: IndexBuffer()
{
	m_glBufferId = -1;
}

GLIndexBuffer::~GLIndexBuffer()
{
	if (m_bMapped)
	{
		unmap();
	}

	free();
}

void GLIndexBuffer::initialize(u32 size, void* pData)
{
	glGenBuffers(1, &m_glBufferId);
	OGL_CHECK_ERROR;
	
	if (size)
	{
		resize(size, pData);
	}
}

void* GLIndexBuffer::map(u32 offset, u32 size, GpuBufferMapType mapType)
{
	B_ASSERT(!m_bMapped);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glBufferId);
	OGL_CHECK_ERROR;
	GLenum glMapType = translateToGL(mapType);

	if (offset == 0)
	{
#ifndef GLES
	m_pMappedData = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, glMapType);
#else
	m_pMappedData = glMapBufferOES(GL_ELEMENT_ARRAY_BUFFER, glMapType);
#endif
	OGL_CHECK_ERROR;
	}
	else
	{
		u32 mapTypeRange = 0;

		if (mapType == GpuBufferMapType::Write)
			mapTypeRange = GL_MAP_WRITE_BIT;
		else if (mapType == GpuBufferMapType::Read)
			mapTypeRange = GL_MAP_READ_BIT;
		else if (mapType == GpuBufferMapType::ReadWrite)
			mapTypeRange = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;

#ifndef GLES
		m_pMappedData = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, offset, size, mapTypeRange);
#else
		//TODO: no range in OES?
		m_pMappedData = glMapBufferOES(GL_ELEMENT_ARRAY_BUFFER, offset, size, mapTypeRange);
#endif
		OGL_CHECK_ERROR;
	}

	B_ASSERT(m_pMappedData);
	m_bMapped = true;

	return m_pMappedData;
}

void GLIndexBuffer::unmap()
{
	B_ASSERT(m_bMapped);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glBufferId);
	OGL_CHECK_ERROR;

#ifndef GLES
	bool bOk = glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER) != 0;
#else
	bool bOk = glUnmapBufferOES(GL_ELEMENT_ARRAY_BUFFER) != 0;
#endif
	OGL_CHECK_ERROR;
	m_bMapped = false;
	m_pMappedData = nullptr;
	B_ASSERT(bOk);
}

void GLIndexBuffer::bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glBufferId);
	OGL_CHECK_ERROR;
}

void GLIndexBuffer::unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	OGL_CHECK_ERROR;
}

void GLIndexBuffer::resize(u32 size, void* pData)
{
	if (m_bMapped)
	{
		unmap();
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glBufferId);
	OGL_CHECK_ERROR;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, pData, m_usage == GpuBufferUsageType::Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	OGL_CHECK_ERROR;
}

void GLIndexBuffer::free()
{
	if (m_glBufferId != -1)
	{
		glDeleteBuffers(1, &m_glBufferId);
		OGL_CHECK_ERROR;
		m_glBufferId = -1;
	}
}

}