// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "opengl_globals.h"
#ifdef _WINDOWS
#include <windows.h>
#endif
#include "graphics/opengl/opengl_gpu_buffer.h"
#include "graphics/opengl/opengl_gpu_program.h"
#include "graphics/gpu_program.h"
#include "base/logger.h"
#include "base/util.h"
#include "base/assert.h"
#include "graphics/texcoord.h"

namespace engine
{
OpenglGpuBuffer::OpenglGpuBuffer()
{
	oglBufferId = ~0;
	mappedOffset = 0;
	mappedDataSize = 0;
}

OpenglGpuBuffer::~OpenglGpuBuffer()
{
	freeGL();
}

void OpenglGpuBuffer::initialize(u32 size, u8* data)
{
	glGenBuffers(1, &oglBufferId);
	CHECK_OPENGL_ERROR;
	
	if (size)
	{
		resize(size, data);
	}
}

u8* OpenglGpuBuffer::map(u32 offset, u32 size, GpuBufferMapType mapType)
{
	B_ASSERT(!mapped);
	glBindBuffer(oglGpuBufferType[(int)type], oglBufferId);
	CHECK_OPENGL_ERROR;
	GLenum oglMapType = openglTranslateEnum(mapType);

	if (offset == 0)
	{
	#ifndef GLES
		mappedData = (u8*)glMapBuffer(oglGpuBufferType[(int)type], oglMapType);
	#else
		mappedData = (u8*)glMapBufferOES(oglGpuBufferType[(int)type], oglMapType);
	#endif
		CHECK_OPENGL_ERROR;
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
		if (glMapBufferRange)
		{
			mappedData = (u8*)glMapBufferRange(oglGpuBufferType[(int)type], offset, size, mapTypeRange);
			CHECK_OPENGL_ERROR;
		}
		else
		{
			oglMappedRangeBuffer.resize(bufferSize);
			mappedOffset = offset;
			mappedDataSize = size;
			mappedData = (u8*)(&oglMappedRangeBuffer[0] + mappedOffset);
		}
	#else
		//TODO: no range in OES?
		mappedData = glMapBufferOES(oglGpuBufferType[(int)type], offset, size, mapTypeRange);
	#endif
		CHECK_OPENGL_ERROR;
	}

	B_ASSERT(mappedData);
	mapped = true;

	return mappedData;
}

void OpenglGpuBuffer::unmap()
{
	B_ASSERT(mapped);
	glBindBuffer(oglGpuBufferType[(int)type], oglBufferId);
	CHECK_OPENGL_ERROR;
	bool ok = true;

#ifndef GLES
	if (mappedOffset && mappedData)
	{
		B_ASSERT(glBufferSubData);
		glBufferSubData(oglGpuBufferType[(int)type], mappedOffset, mappedDataSize, (u8*)mappedData);
		CHECK_OPENGL_ERROR;
		mappedOffset = 0;
		mappedDataSize = 0;
	}
	else
	{
		ok = glUnmapBuffer(oglGpuBufferType[(int)type]) != 0;
	}
#else
	ok = glUnmapBufferOES(oglGpuBufferType[(int)type]) != 0;
#endif
	CHECK_OPENGL_ERROR;
	
	mapped = false;
	mappedData = nullptr;
	B_ASSERT(ok);
}

void OpenglGpuBuffer::bind()
{
	glBindBuffer(oglGpuBufferType[(int)type], oglBufferId);
	CHECK_OPENGL_ERROR;
}

void OpenglGpuBuffer::unbind()
{
	glBindBuffer(oglGpuBufferType[(int)type], 0);
	CHECK_OPENGL_ERROR;
}

void OpenglGpuBuffer::resize(u32 newSize, u8* newData)
{
	if (mapped)
	{
		unmap();
	}

	GLenum bufType = oglGpuBufferType[(int)type];
	
	glBindBuffer(bufType, oglBufferId);
	CHECK_OPENGL_ERROR;
	glBufferData(bufType, newSize, newData, usage == GpuBufferUsageType::Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	CHECK_OPENGL_ERROR;
	bufferSize = newSize;
}

void OpenglGpuBuffer::setupVertexDeclarationGL(OpenglGpuProgram& program, u32 offset)
{
	#define OGL_VBUFFER_OFFSET(i) ((void*)(i))

	glBindBuffer(oglGpuBufferType[(int)type], oglBufferId);
	CHECK_OPENGL_ERROR;
	u32 stride = desc.getStride();
	
	for (size_t i = 0; i < desc.getElements().size(); ++i)
	{
		auto elem = desc.getElements()[i];
		OpenGLVertexAttribute* attr = program.findAttributeGL(elem.type, elem.index);
		
		if (!attr)
		{
			continue;
		}

		u32 offsetSum = (offset * stride) + elem.offset;

		glEnableVertexAttribArray(attr->location);
		CHECK_OPENGL_ERROR;
		glVertexAttribPointer(
			attr->location, elem.componentCount, openglTranslateEnum(elem.componentType), GL_FALSE,
			stride, OGL_VBUFFER_OFFSET(offsetSum));
		CHECK_OPENGL_ERROR;
		
		if (glVertexAttribDivisor)
			glVertexAttribDivisor(attr->location, 0);

		CHECK_OPENGL_ERROR;
	}
}

void OpenglGpuBuffer::freeGL()
{
	if (oglBufferId != -1)
	{
		glDeleteBuffers(1, &oglBufferId);
		CHECK_OPENGL_ERROR;
		oglBufferId = -1;
	}
}

#ifdef _DEBUG
void OpenglGpuBuffer::debug()
{
	u8* data = (u8*)map(0, 0, GpuBufferMapType::Read);

	B_ASSERT(data);

	if (type == GpuBufferType::Indices)
	{
		B_LOG_DEBUG("Index buffer:");
	}
	else if (type == GpuBufferType::Vertices)
	{
		B_LOG_DEBUG("Vertex buffer:");
	}

	for (size_t i = 0; i < elementCount; i++)
	{
		for (auto& el : desc.getElements())
		{
			switch (el.type)
			{
			case GpuBufferElementType::Index:
				B_LOG_DEBUG("Index: " << *(VertexIndexType*)data);
				break;
			case GpuBufferElementType::Position:
				B_LOG_DEBUG("Pos: " << ((Vec3*)data)->x << "," << ((Vec3*)data)->y << "," << ((Vec3*)data)->z);
				break;
			case GpuBufferElementType::Normal:
				B_LOG_DEBUG("Nrm: " << ((Vec3*)data)->x << "," << ((Vec3*)data)->y << "," << ((Vec3*)data)->z);
				break;
			case GpuBufferElementType::Tangent:
				B_LOG_DEBUG("Tan: " << ((Vec3*)data)->x << "," << ((Vec3*)data)->y << "," << ((Vec3*)data)->z);
				break;
			case GpuBufferElementType::Bitangent:
				B_LOG_DEBUG("Bit: " << "," << ((Vec3*)data)->x << "," << ((Vec3*)data)->y << "," << ((Vec3*)data)->z);
				break;
			case GpuBufferElementType::Color:
				B_LOG_DEBUG("Col: " << ((Color*)data)->r << "," << ((Color*)data)->g << "," << ((Color*)data)->b << "," << ((Color*)data)->a);
				break;
			case GpuBufferElementType::TexCoord:
				B_LOG_DEBUG("UV: " << ((TexCoord*)data)->x << "," << ((TexCoord*)data)->y);
				break;
			default:
				break;
			}

			data += el.getElementSize();
		}
	}

	unmap();
}
#endif

}